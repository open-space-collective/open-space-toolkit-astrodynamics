/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

#include <Eigen/Eigenvalues>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Index;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector2d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;

namespace
{

// Mean orbital elements [a, e, i, Ω, ω, M]. Uses the virtual anomaly getters, as COE::getSIVector assumes that the
// stored anomaly is a true anomaly, which does not hold for Brouwer-Lyddane mean elements.
Vector6d MeanCOEVector(const COE& aCOE)
{
    Vector6d meanCOEVector;
    meanCOEVector << aCOE.getSemiMajorAxis().inMeters(), aCOE.getEccentricity(), aCOE.getInclination().inRadians(),
        aCOE.getRaan().inRadians(), aCOE.getAop().inRadians(), aCOE.getMeanAnomaly().inRadians();

    return meanCOEVector;
}

Vector6d ComputeRelativeOrbitalElements_(const Vector6d& aMeanCOEVector, const Vector6d& aReferenceMeanCOEVector)
{
    const auto wrapAngle = [](const double& anAngle) -> double
    {
        return std::remainder(anAngle, 2.0 * M_PI);
    };

    const Vector6d& coe = aMeanCOEVector;
    const Vector6d& referenceCOE = aReferenceMeanCOEVector;

    const double deltaRaan = wrapAngle(coe[3] - referenceCOE[3]);

    Vector6d relativeOrbitalElements;
    relativeOrbitalElements << (coe[0] - referenceCOE[0]) / referenceCOE[0],
        wrapAngle((coe[4] + coe[5]) - (referenceCOE[4] + referenceCOE[5]) + deltaRaan * std::cos(referenceCOE[2])),
        coe[1] * std::cos(coe[4]) - referenceCOE[1] * std::cos(referenceCOE[4]),
        coe[1] * std::sin(coe[4]) - referenceCOE[1] * std::sin(referenceCOE[4]), coe[2] - referenceCOE[2],
        deltaRaan * std::sin(referenceCOE[2]);

    return relativeOrbitalElements;
}

// Secular J2 quantities of a reference orbit, following the notation of Koenig, Guffanti and D'Amico (2017)
struct J2Quantities
{
    double n;         // Keplerian mean motion
    double eta;       // sqrt(1 - e^2)
    double kappa;     // 3/4 J2 R^2 sqrt(mu) / (a^(7/2) eta^4)
    double E;         // 1 + eta
    double F;         // 4 + 3 eta
    double G;         // 1 / eta^2
    double P;         // 3 cos^2(i) - 1
    double Q;         // 5 cos^2(i) - 1
    double S;         // sin(2 i)
    double T;         // sin^2(i)
    double raanRate;  // dΩ/dt
    double aopRate;   // dω/dt
    double meanAnomalyRate;
};

J2Quantities ComputeJ2Quantities(const Vector6d& aMeanCOEVector)
{
    static const double mu =
        EarthGravitationalModel::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());
    static const double equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();
    static const double J2 = EarthGravitationalModel::EGM2008.J2_;

    const double a = aMeanCOEVector[0];
    const double e = aMeanCOEVector[1];
    const double cosI = std::cos(aMeanCOEVector[2]);

    J2Quantities quantities;

    quantities.n = std::sqrt(mu / (a * a * a));
    quantities.eta = std::sqrt(1.0 - e * e);
    quantities.kappa = 0.75 * J2 * equatorialRadius * equatorialRadius * std::sqrt(mu) /
                       (std::pow(a, 3.5) * std::pow(quantities.eta, 4.0));
    quantities.E = 1.0 + quantities.eta;
    quantities.F = 4.0 + 3.0 * quantities.eta;
    quantities.G = 1.0 / (quantities.eta * quantities.eta);
    quantities.P = 3.0 * cosI * cosI - 1.0;
    quantities.Q = 5.0 * cosI * cosI - 1.0;
    quantities.S = std::sin(2.0 * aMeanCOEVector[2]);
    quantities.T = 1.0 - cosI * cosI;
    quantities.raanRate = -2.0 * quantities.kappa * cosI;
    quantities.aopRate = quantities.kappa * quantities.Q;
    quantities.meanAnomalyRate = quantities.n + quantities.kappa * quantities.eta * quantities.P;

    return quantities;
}

// The J2 quantities only depend on the semi-major axis, the eccentricity and the inclination, which have no secular J2
// rate: they are shared by all the transitions from a propagated reference. The eccentricity vector of the reference
// at the start and at the end of the transition, and the cosine and sine of its rotation over the transition, are
// provided.
Matrix6d ComputeStateTransitionMatrix_(
    const J2Quantities& aJ2Quantities,
    const Vector2d& anInitialEccentricityVector,
    const Vector2d& aFinalEccentricityVector,
    const double& aCosRotation,
    const double& aSinRotation,
    const double& aTau
)
{
    const J2Quantities& q = aJ2Quantities;

    const double exi = anInitialEccentricityVector[0];
    const double eyi = anInitialEccentricityVector[1];
    const double exf = aFinalEccentricityVector[0];
    const double eyf = aFinalEccentricityVector[1];

    const double kt = q.kappa * aTau;

    Matrix6d stm = Matrix6d::Identity();

    stm(1, 0) = -(1.5 * q.n + 3.5 * q.kappa * q.E * q.P) * aTau;
    stm(1, 2) = kt * exi * q.F * q.G * q.P;
    stm(1, 3) = kt * eyi * q.F * q.G * q.P;
    stm(1, 4) = -kt * q.F * q.S;

    stm(2, 0) = 3.5 * kt * eyf * q.Q;
    stm(2, 2) = aCosRotation - 4.0 * kt * exi * eyf * q.G * q.Q;
    stm(2, 3) = -aSinRotation - 4.0 * kt * eyi * eyf * q.G * q.Q;
    stm(2, 4) = 5.0 * kt * eyf * q.S;

    stm(3, 0) = -3.5 * kt * exf * q.Q;
    stm(3, 2) = aSinRotation + 4.0 * kt * exi * exf * q.G * q.Q;
    stm(3, 3) = aCosRotation + 4.0 * kt * eyi * exf * q.G * q.Q;
    stm(3, 4) = -5.0 * kt * exf * q.S;

    stm(5, 0) = 3.5 * kt * q.S;
    stm(5, 2) = -4.0 * kt * exi * q.G * q.S;
    stm(5, 3) = -4.0 * kt * eyi * q.G * q.S;
    stm(5, 4) = 2.0 * kt * q.T;

    return stm;
}

// Eccentricity vector of mean orbital elements [a, e, i, Ω, ω, M]
Vector2d EccentricityVector(const Vector6d& aMeanCOEVector)
{
    return aMeanCOEVector[1] * Vector2d(std::cos(aMeanCOEVector[4]), std::sin(aMeanCOEVector[4]));
}

Matrix6d ComputeStateTransitionMatrix_(const Vector6d& aMeanCOEVector, const double& aTau)
{
    const J2Quantities q = ComputeJ2Quantities(aMeanCOEVector);

    Vector6d finalMeanCOEVector = aMeanCOEVector;
    finalMeanCOEVector[4] += q.aopRate * aTau;

    return ComputeStateTransitionMatrix_(
        q,
        EccentricityVector(aMeanCOEVector),
        EccentricityVector(finalMeanCOEVector),
        std::cos(q.aopRate * aTau),
        std::sin(q.aopRate * aTau),
        aTau
    );
}

// Effect on the ROE of a constant relative semi-major axis rate (dimensionless, per second), obtained by integrating
// the δa column of the J2 state transition matrix
Vector6d ComputeSemiMajorAxisRateEffect(const Vector6d& aMeanCOEVector, const double& aTau)
{
    const J2Quantities q = ComputeJ2Quantities(aMeanCOEVector);

    const double e = aMeanCOEVector[1];
    const double aopFinal = aMeanCOEVector[4] + q.aopRate * aTau;
    const double tau2 = aTau * aTau;

    Vector6d effect;
    effect << aTau, -(0.75 * q.n + 1.75 * q.kappa * q.E * q.P) * tau2,
        1.75 * q.kappa * e * std::sin(aopFinal) * q.Q * tau2, -1.75 * q.kappa * e * std::cos(aopFinal) * q.Q * tau2,
        0.0, 1.75 * q.kappa * q.S * tau2;

    return effect;
}

// Solve
//     minimize ||A u - b||^2 + w * sum_k ||u_k||   subject to ||u_k|| <= 1
// where u_k are the consecutive GroupSize-vectors of u, with the Alternating Direction Method of Multipliers. The
// returned solution is exactly sparse and feasible.
template <int GroupSize>
VectorXd SolveGroupLasso(const MatrixXd& A, const VectorXd& b, const double& aWeight, const VectorXd& aWarmStart)
{
    static const Size maximumIterationCount = 20000;
    static const double absoluteTolerance = 1e-7;
    static const double relativeTolerance = 1e-6;

    using GroupVector = Eigen::Matrix<double, GroupSize, 1>;

    const Index size = A.cols();
    const Index groupCount = size / GroupSize;

    const MatrixXd hessian = 2.0 * A.transpose() * A;
    const VectorXd gradientOffset = 2.0 * A.transpose() * b;

    double rho = std::max(hessian.trace() / static_cast<double>(size), 1e-12);
    Eigen::LLT<MatrixXd> factorization(hessian + rho * MatrixXd::Identity(size, size));

    VectorXd u = aWarmStart;
    VectorXd z = aWarmStart;
    VectorXd y = VectorXd::Zero(size);

    for (Size iteration = 0; iteration < maximumIterationCount; ++iteration)
    {
        u = factorization.solve(gradientOffset + rho * (z - y));

        const VectorXd previousZ = z;

        for (Index group = 0; group < groupCount; ++group)
        {
            const GroupVector v = u.segment<GroupSize>(GroupSize * group) + y.segment<GroupSize>(GroupSize * group);
            const double vNorm = v.norm();

            GroupVector zGroup = (vNorm > 0.0) ? GroupVector(std::max(0.0, 1.0 - aWeight / (rho * vNorm)) * v)
                                               : GroupVector(GroupVector::Zero());

            const double zGroupNorm = zGroup.norm();
            if (zGroupNorm > 1.0)
            {
                zGroup /= zGroupNorm;
            }

            z.segment<GroupSize>(GroupSize * group) = zGroup;
        }

        y += u - z;

        const double primalResidual = (u - z).norm();
        const double dualResidual = rho * (z - previousZ).norm();

        if ((primalResidual <= absoluteTolerance * std::sqrt(size) + relativeTolerance * std::max(u.norm(), z.norm())
            ) &&
            (dualResidual <= absoluteTolerance * std::sqrt(size) + relativeTolerance * rho * y.norm()))
        {
            break;
        }

        // Balance the residuals by adapting the penalty
        if ((iteration % 25) == 24)
        {
            double scaling = 1.0;

            if (primalResidual > 10.0 * dualResidual)
            {
                scaling = 2.0;
            }
            else if (dualResidual > 10.0 * primalResidual)
            {
                scaling = 0.5;
            }

            if (scaling != 1.0)
            {
                rho *= scaling;
                y /= scaling;
                factorization.compute(hessian + rho * MatrixXd::Identity(size, size));
            }
        }
    }

    return z;
}

// Solve
//     minimize u^T M u + 2 g^T u + w ||u||   subject to ||u|| <= 1
// for a single GroupSize-vector u, M being symmetric positive semi-definite: the single group version of
// SolveGroupLasso (with M = A^T A and g = -A^T b), solved exactly. The minimizer is u(λ) = -(M + λ I)^-1 g, with
// λ = w / (2 ||u||) when the norm constraint is inactive, and ||u(λ)|| = 1 otherwise. Both equations are solved by
// Newton iterations on 1 / ||u(λ)||, concave and increasing with λ, so that the iterations converge monotonically.
template <int GroupSize>
Eigen::Matrix<double, GroupSize, 1> SolveSingleGroupLasso(
    const Eigen::Matrix<double, GroupSize, GroupSize>& M,
    const Eigen::Matrix<double, GroupSize, 1>& g,
    const double& aWeight
)
{
    using GroupVector = Eigen::Matrix<double, GroupSize, 1>;
    using GroupArray = Eigen::Array<double, GroupSize, 1>;

    static const Size maximumIterationCount = 50;
    static const double relativeTolerance = 1e-12;

    const double gradientNorm = g.norm();

    // Zero is optimal when the gradient at zero lies within the subdifferential of the weight term
    if (!(2.0 * gradientNorm > aWeight))
    {
        return GroupVector::Zero();
    }

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix<double, GroupSize, GroupSize>> eigenSolver;
    eigenSolver.computeDirect(M);

    const GroupArray eigenvalues = eigenSolver.eigenvalues().array().max(0.0);
    const GroupArray projectedGradient = (eigenSolver.eigenvectors().transpose() * g).array();

    // 1 / ||u(λ)||, and its derivative
    const auto computeInverseNorm = [&eigenvalues,
                                     &projectedGradient](const double& aLambda, double& aDerivative) -> double
    {
        const GroupArray inverse = (eigenvalues + aLambda).inverse();
        const double squaredNorm = (projectedGradient * inverse).square().sum();
        const double norm = std::sqrt(squaredNorm);

        aDerivative = (projectedGradient.square() * inverse.cube()).sum() / (squaredNorm * norm);

        return 1.0 / norm;
    };

    // Newton iterations on f(λ) = 1 / ||u(λ)|| - h(λ), concave, from a side of the root where f <= 0
    const auto solve = [&computeInverseNorm](double lambda, const double& aSlope, const double& anOffset) -> double
    {
        for (Size iteration = 0; iteration < maximumIterationCount; ++iteration)
        {
            double derivative;
            const double value = computeInverseNorm(lambda, derivative) - (aSlope * lambda + anOffset);
            const double step = value / (derivative - aSlope);

            if (!std::isfinite(step) || (std::abs(step) <= (relativeTolerance * lambda)))
            {
                break;
            }

            lambda = std::max(lambda - step, std::numeric_limits<double>::min());
        }

        return lambda;
    };

    // λ ||u(λ)|| = w / 2, i.e. 1 / ||u(λ)|| = 2 λ / w: from the right of the root (decreasing iterates)
    const double lambdaUpperBound =
        std::max(eigenvalues.maxCoeff() * aWeight / (2.0 * gradientNorm - aWeight), std::numeric_limits<double>::min());

    double lambda = (aWeight > 0.0) ? solve(lambdaUpperBound, 2.0 / aWeight, 0.0) : std::numeric_limits<double>::min();

    // ||u(λ)|| = 1: from the left of the root (increasing iterates)
    double derivative;
    if (computeInverseNorm(lambda, derivative) < 1.0)
    {
        lambda = solve(lambda, 0.0, 1.0);
    }

    return -eigenSolver.eigenvectors() * (projectedGradient / (eigenvalues + lambda)).matrix();
}

// Remainder of the division of x by y > 0, in [0, y)
double PositiveRemainder(const double& x, const double& y)
{
    const double remainder = std::fmod(x, y);

    return (remainder < 0.0) ? (remainder + y) : remainder;
}

Matrix3d RTNToFrame(const Vector3d& aPositionCoordinates, const Vector3d& aVelocityCoordinates)
{
    const Vector3d radial = aPositionCoordinates.normalized();
    const Vector3d normal = aPositionCoordinates.cross(aVelocityCoordinates).normalized();
    const Vector3d tangential = normal.cross(radial);

    Matrix3d rotation;
    rotation.col(0) = radial;
    rotation.col(1) = tangential;
    rotation.col(2) = normal;

    return rotation;
}

// Burn plan: the middle, the duration and the normalized delta-v (delta-v / maximum delta-v) of each burn, the
// normalized delta-v of the burns being concatenated
struct BurnPlanSolution
{
    VectorXd burnMidpointOffsets;
    VectorXd burnDurations;
    VectorXd normalizedDeltaVelocities;
    double cost;
    Size iterationCount;
};

// Burn planning from a planning instant. The prediction samples, the weights and the free motion are shared by all
// the plans compared by the effectivity.
//
// The delta-v of a burn has ControlDimension components, along the last ControlDimension axes of the RTN frame: the
// tangential and normal axes (2), and the radial axis when radial thrust is allowed (3).
//
// A burn is placed by its middle, which sets the argument of latitude of the burn arc. The middle of each burn after
// the first lies within one orbit (one period of the mean argument of latitude) after the earliest middle allowed by
// the previous burn, one burn spacing after its middle. The first burn starts within a window (fixed start when the
// window is empty). When the duration of a burn changes, its middle does not move, except when its start is fixed.
// When a burn moves, the following burns keep their argument of latitude, and move by whole orbits if needed to stay
// within their windows.
template <int ControlDimension>
class BurnPlanner
{
   public:
    using Solution = BurnPlanSolution;

    BurnPlanner(
        const ROETargeting::Parameters& aParameters,
        const Vector6d& aReferenceMeanCOEVector,
        const Vector6d& aRelativeOrbitalElements,
        const double& aMeanArgumentOfLatitude,
        const Vector6d& aTargetRelativeOrbitalElements
    )
        : parameters_(aParameters),
          referenceMeanCOEVector_(aReferenceMeanCOEVector),
          relativeOrbitalElements_(aRelativeOrbitalElements),
          meanArgumentOfLatitude_(aMeanArgumentOfLatitude),
          j2Quantities_(ComputeJ2Quantities(aReferenceMeanCOEVector)),
          meanArgumentOfLatitudeRate_(j2Quantities_.aopRate + j2Quantities_.meanAnomalyRate),
          meanArgumentOfLatitudePeriod_(2.0 * M_PI / meanArgumentOfLatitudeRate_),
          burnCount_(aParameters.horizonBurnCount),
          sampleCount_(2 * aParameters.horizonBurnCount),
          burnSpacing_(aParameters.burnSpacing.inSeconds()),
          maximumBurnDuration_(aParameters.maximumBurnDuration.inSeconds()),
          maximumDeltaVelocity_(aParameters.thrustAcceleration * aParameters.maximumBurnDuration.inSeconds())
    {
        const double semiMajorAxis = referenceMeanCOEVector_[0];

        // Samples halfway through and at the end of each burn spacing after the planning instant, common to all the
        // plans
        sampleOffsets_.resize(sampleCount_);
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            sampleOffsets_[2 * burnIndex] = (burnIndex + 0.5) * burnSpacing_;
            sampleOffsets_[2 * burnIndex + 1] = (burnIndex + 1.0) * burnSpacing_;
        }

        // Eccentricity vector of the reference at the samples, and its rotation since the planning instant
        eccentricityVector_ = EccentricityVector(referenceMeanCOEVector_);
        sampleEccentricityVectors_.resize(2, sampleCount_);
        sampleRotations_.resize(2, sampleCount_);
        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            const double rotation = j2Quantities_.aopRate * sampleOffsets_[sampleIndex];

            sampleRotations_.col(sampleIndex) << std::cos(rotation), std::sin(rotation);
            sampleEccentricityVectors_.col(sampleIndex) =
                Rotation2D(sampleRotations_.col(sampleIndex)) * eccentricityVector_;
        }

        // Weights: tolerance-normalized errors, averaged over the samples
        weights_ = (semiMajorAxis / parameters_.toleranceVector.array()).matrix() /
                   std::sqrt(static_cast<double>(sampleCount_));

        const Vector6d targetRelativeOrbitalElements = aTargetRelativeOrbitalElements / semiMajorAxis;
        const double relativeSemiMajorAxisRate = parameters_.relativeSemiMajorAxisRate / semiMajorAxis;

        // Free motion at the samples, in weighted least-squares form
        weightedTarget_.resize(6 * sampleCount_);
        freeMotion_.resize(6, sampleCount_);
        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            freeMotion_.col(sampleIndex) =
                ComputeStateTransitionMatrix_(referenceMeanCOEVector_, sampleOffsets_[sampleIndex]) *
                    relativeOrbitalElements_ +
                ComputeSemiMajorAxisRateEffect(referenceMeanCOEVector_, sampleOffsets_[sampleIndex]) *
                    relativeSemiMajorAxisRate;

            weightedTarget_.segment<6>(6 * sampleIndex) =
                weights_.cwiseProduct(targetRelativeOrbitalElements - freeMotion_.col(sampleIndex));
        }
    }

    // The plan whose first burn starts after a delay, the following burns at their best arguments of latitude
    Solution solve(const double& aFirstBurnDelay) const
    {
        Solution solution = {
            VectorXd::Zero(burnCount_),
            VectorXd::Zero(burnCount_),
            VectorXd::Zero(ControlDimension * burnCount_),
            std::numeric_limits<double>::infinity(),
            0,
        };

        // Start from the earliest burns, without any delta-v: the first sweep then adds the burns one at a time
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            solution.burnMidpointOffsets[burnIndex] = aFirstBurnDelay + burnIndex * burnSpacing_;
        }

        optimize_(true, {aFirstBurnDelay, 0.0}, -std::numeric_limits<double>::infinity(), solution);

        return solution;
    }

    // The plan of a solution whose first burn also starts at its best start time, within one orbit after the start of
    // the first burn of that solution. Stops as soon as the cost drops below the provided one.
    Solution solveFirstBurnStart(const Solution& aSolution, const double& aStopCost) const
    {
        Solution solution = aSolution;

        optimize_(true, {computeBurnStartOffset_(aSolution, 0), meanArgumentOfLatitudePeriod_}, aStopCost, solution);

        return solution;
    }

    // The plan of a solution without its first burn
    Solution solveWithoutFirstBurn(const Solution& aSolution) const
    {
        const FirstBurnWindow firstBurnWindow = {computeBurnStartOffset_(aSolution, 0), 0.0};

        Solution solution = aSolution;
        solution.normalizedDeltaVelocities.segment<ControlDimension>(0).setZero();
        solution.cost = std::numeric_limits<double>::infinity();

        setBurnDuration_(0, 0.0, firstBurnWindow, solution);

        optimize_(false, firstBurnWindow, -std::numeric_limits<double>::infinity(), solution);

        return solution;
    }

    ROETargeting::Plan toPlan(const Instant& anEpoch, const Solution& aSolution) const
    {
        const double semiMajorAxis = referenceMeanCOEVector_[0];

        const VectorXd predictedResponse =
            computeWeightedResponseMatrix_(true, aSolution) * aSolution.normalizedDeltaVelocities;

        ROETargeting::Plan plan = {
            anEpoch,
            semiMajorAxis,
            relativeOrbitalElements_ * semiMajorAxis,
            Array<Interval>::Empty(),
            Array<Vector3d>::Empty(),
            Array<Instant>::Empty(),
            Array<Vector6d>::Empty(),
            aSolution.cost,
            aSolution.iterationCount,
        };

        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            const Instant burnStart = anEpoch + Duration::Seconds(computeBurnStartOffset_(aSolution, burnIndex));

            plan.burnIntervals.add(
                Interval::Closed(burnStart, burnStart + Duration::Seconds(aSolution.burnDurations[burnIndex]))
            );
            plan.deltaVelocities.add(getNormalizedDeltaVelocity(aSolution, burnIndex) * maximumDeltaVelocity_);
        }

        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            plan.predictionInstants.add(anEpoch + Duration::Seconds(sampleOffsets_[sampleIndex]));
            plan.predictedRelativeOrbitalElements.add(
                (freeMotion_.col(sampleIndex) +
                 weights_.cwiseInverse().cwiseProduct(predictedResponse.segment<6>(6 * sampleIndex))) *
                semiMajorAxis
            );
        }

        return plan;
    }

    // Normalized delta-v of a burn, in the RTN frame
    Vector3d getNormalizedDeltaVelocity(const Solution& aSolution, const Index& aBurnIndex) const
    {
        Vector3d normalizedDeltaVelocity = Vector3d::Zero();
        normalizedDeltaVelocity.tail<ControlDimension>() =
            aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * aBurnIndex);

        return normalizedDeltaVelocity;
    }

   private:
    using ControlVector = Eigen::Matrix<double, ControlDimension, 1>;
    using ControlMatrix = Eigen::Matrix<double, ControlDimension, ControlDimension>;
    using BurnResponse = Eigen::Matrix<double, Eigen::Dynamic, ControlDimension>;

    // The first burn starts between start and start + length
    struct FirstBurnWindow
    {
        double start;
        double length;
    };

    const ROETargeting::Parameters& parameters_;
    const Vector6d referenceMeanCOEVector_;
    const Vector6d relativeOrbitalElements_;
    const double meanArgumentOfLatitude_;
    const J2Quantities j2Quantities_;
    const double meanArgumentOfLatitudeRate_;
    const double meanArgumentOfLatitudePeriod_;
    const Index burnCount_;
    const Index sampleCount_;
    const double burnSpacing_;
    const double maximumBurnDuration_;
    const double maximumDeltaVelocity_;

    VectorXd sampleOffsets_;
    Vector2d eccentricityVector_;
    Eigen::Matrix2Xd sampleEccentricityVectors_;
    Eigen::Matrix2Xd sampleRotations_;
    Vector6d weights_;
    MatrixXd freeMotion_;
    VectorXd weightedTarget_;

    // Rotation matrix from its cosine and sine
    static Eigen::Matrix2d Rotation2D(const Vector2d& aCosineSine)
    {
        Eigen::Matrix2d rotation;
        rotation << aCosineSine[0], -aCosineSine[1], aCosineSine[1], aCosineSine[0];

        return rotation;
    }

    double computeBurnStartOffset_(const Solution& aSolution, const Index& aBurnIndex) const
    {
        return aSolution.burnMidpointOffsets[aBurnIndex] - 0.5 * aSolution.burnDurations[aBurnIndex];
    }

    // Weighted response at the samples to the normalized delta-v (delta-v / maximum delta-v) of a burn
    void computeWeightedBurnResponse_(
        const double& aBurnMidpoint, const double& aBurnDuration, BurnResponse& aBurnResponse
    ) const
    {
        const double burnMeanArgumentOfLatitude = meanArgumentOfLatitude_ + meanArgumentOfLatitudeRate_ * aBurnMidpoint;
        const double halfBurnArc = 0.5 * meanArgumentOfLatitudeRate_ * aBurnDuration;

        // Columns of the control input matrix along the thrust axes
        const Eigen::Matrix<double, 6, ControlDimension> controlInputMatrix =
            ROETargeting::ComputeControlInputMatrix(
                referenceMeanCOEVector_[0],
                burnMeanArgumentOfLatitude - halfBurnArc,
                burnMeanArgumentOfLatitude + halfBurnArc
            )
                .rightCols<ControlDimension>() *
            maximumDeltaVelocity_;

        // Rotation of the eccentricity vector of the reference from the planning instant to the burn
        const double rotation = j2Quantities_.aopRate * aBurnMidpoint;
        const Vector2d burnRotation = {std::cos(rotation), std::sin(rotation)};
        const Vector2d burnEccentricityVector = Rotation2D(burnRotation) * eccentricityVector_;

        aBurnResponse.setZero(6 * sampleCount_, ControlDimension);

        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            if (sampleOffsets_[sampleIndex] < aBurnMidpoint)
            {
                continue;
            }

            // Rotation from the burn to the sample
            const Vector2d sampleRotation = Rotation2D(burnRotation).transpose() * sampleRotations_.col(sampleIndex);

            aBurnResponse.template middleRows<6>(6 * sampleIndex) = weights_.asDiagonal() *
                                                                    ComputeStateTransitionMatrix_(
                                                                        j2Quantities_,
                                                                        burnEccentricityVector,
                                                                        sampleEccentricityVectors_.col(sampleIndex),
                                                                        sampleRotation[0],
                                                                        sampleRotation[1],
                                                                        sampleOffsets_[sampleIndex] - aBurnMidpoint
                                                                    ) *
                                                                    controlInputMatrix;
        }
    }

    BurnResponse computeWeightedBurnResponse_(const double& aBurnMidpoint, const double& aBurnDuration) const
    {
        BurnResponse burnResponse;
        computeWeightedBurnResponse_(aBurnMidpoint, aBurnDuration, burnResponse);

        return burnResponse;
    }

    // Weighted response at the samples to the normalized delta-v (delta-v / maximum delta-v) of each burn
    MatrixXd computeWeightedResponseMatrix_(const bool& includeFirstBurn, const Solution& aSolution) const
    {
        MatrixXd responseMatrix = MatrixXd::Zero(6 * sampleCount_, ControlDimension * burnCount_);

        for (Index burnIndex = (includeFirstBurn ? 0 : 1); burnIndex < burnCount_; ++burnIndex)
        {
            responseMatrix.middleCols<ControlDimension>(ControlDimension * burnIndex) = computeWeightedBurnResponse_(
                aSolution.burnMidpointOffsets[burnIndex], aSolution.burnDurations[burnIndex]
            );
        }

        return responseMatrix;
    }

    // Window of the middle of a burn: [start, start + length]
    std::pair<double, double> computeBurnMidpointWindow_(
        const Index& aBurnIndex, const FirstBurnWindow& aFirstBurnWindow, const Solution& aSolution
    ) const
    {
        if (aBurnIndex == 0)
        {
            return {aFirstBurnWindow.start + 0.5 * aSolution.burnDurations[0], aFirstBurnWindow.length};
        }

        return {aSolution.burnMidpointOffsets[aBurnIndex - 1] + burnSpacing_, meanArgumentOfLatitudePeriod_};
    }

    // Change the duration of a burn, keeping its middle, or its start when the start is fixed (first burn with an
    // empty window). The first burn still starts within its window.
    void setBurnDuration_(
        const Index& aBurnIndex,
        const double& aBurnDuration,
        const FirstBurnWindow& aFirstBurnWindow,
        Solution& aSolution
    ) const
    {
        if (aBurnIndex == 0)
        {
            const double burnStart = std::clamp(
                aSolution.burnMidpointOffsets[0] - 0.5 * aBurnDuration,
                aFirstBurnWindow.start,
                aFirstBurnWindow.start + aFirstBurnWindow.length
            );

            aSolution.burnMidpointOffsets[0] = burnStart + 0.5 * aBurnDuration;
        }

        aSolution.burnDurations[aBurnIndex] = aBurnDuration;
    }

    // Move the burns following a burn by whole orbits, if needed to keep them within their windows. Returns whether a
    // burn moved.
    bool keepFollowingBurnsInWindows_(const Index& aBurnIndex, Solution& aSolution) const
    {
        bool burnMoved = false;

        for (Index burnIndex = aBurnIndex + 1; burnIndex < burnCount_; ++burnIndex)
        {
            const double windowStart = aSolution.burnMidpointOffsets[burnIndex - 1] + burnSpacing_;
            const double midpoint = aSolution.burnMidpointOffsets[burnIndex];

            if ((midpoint >= windowStart) && (midpoint < (windowStart + meanArgumentOfLatitudePeriod_)))
            {
                continue;
            }

            aSolution.burnMidpointOffsets[burnIndex] =
                windowStart + PositiveRemainder(midpoint - windowStart, meanArgumentOfLatitudePeriod_);

            burnMoved = true;
        }

        return burnMoved;
    }

    // Alternate sweeps over the burn arguments of latitude with joint delta-v optimizations, keeping the best solution
    void optimize_(
        const bool& includeFirstBurn,
        const FirstBurnWindow& aFirstBurnWindow,
        const double& aStopCost,
        Solution& aSolution
    ) const
    {
        static const Size maximumSweepCount = 4;

        Solution bestSolution = aSolution;

        for (Size sweepIndex = 0; sweepIndex < maximumSweepCount; ++sweepIndex)
        {
            const bool burnsMoved = sweepBurns_(includeFirstBurn, aFirstBurnWindow, aSolution);

            // Without moved burns, the joint optimization would return the current solution
            if (!burnsMoved && std::isfinite(bestSolution.cost))
            {
                break;
            }

            // A burn whose duration changes moves the windows of the following burns, which can move by whole orbits:
            // the next sweep then searches their windows again
            const bool burnsMovedByDurations = optimizeDeltaVelocities_(includeFirstBurn, aFirstBurnWindow, aSolution);

            Solution finalizedSolution = aSolution;
            finalize_(includeFirstBurn, aFirstBurnWindow, finalizedSolution);

            if (finalizedSolution.cost < bestSolution.cost)
            {
                bestSolution = finalizedSolution;
            }

            if (!(burnsMoved || burnsMovedByDurations) || (bestSolution.cost < aStopCost))
            {
                break;
            }
        }

        bestSolution.iterationCount = aSolution.iterationCount;

        aSolution = bestSolution;
    }

    // Block coordinate descent over the burns: in turn, the middle and the delta-v of each burn are optimized, the
    // other burns being fixed. The middle is searched among candidates evenly spread over the window of the burn, then
    // refined by golden section search, the delta-v being solved exactly for each candidate. The burn durations, which
    // set the burn arcs, are frozen. Returns whether a burn moved.
    bool sweepBurns_(const bool& includeFirstBurn, const FirstBurnWindow& aFirstBurnWindow, Solution& aSolution) const
    {
        static const double midpointTolerance = 1.0;  // [s]
        static const Size refinementIterationCount = 12;
        static const double goldenRatio = 0.5 * (std::sqrt(5.0) - 1.0);

        bool burnsMoved = false;

        VectorXd residual =
            computeWeightedResponseMatrix_(includeFirstBurn, aSolution) * aSolution.normalizedDeltaVelocities -
            weightedTarget_;

        for (Index burnIndex = (includeFirstBurn ? 0 : 1); burnIndex < burnCount_; ++burnIndex)
        {
            const double burnDuration = aSolution.burnDurations[burnIndex];
            const double currentMidpoint = aSolution.burnMidpointOffsets[burnIndex];

            const std::pair<double, double> window = computeBurnMidpointWindow_(burnIndex, aFirstBurnWindow, aSolution);
            const double windowStart = window.first;
            const double windowLength = window.second;

            const VectorXd otherResidual =
                residual -
                computeWeightedBurnResponse_(currentMidpoint, burnDuration) *
                    aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * burnIndex);

            double bestMidpoint = currentMidpoint;
            double bestCost = std::numeric_limits<double>::infinity();
            ControlVector bestNormalizedDeltaVelocity = ControlVector::Zero();

            BurnResponse burnResponse(6 * sampleCount_, ControlDimension);

            // Cost of the burn at a candidate middle with its best delta-v, up to a constant
            const auto evaluate = [&](const double& aMidpoint) -> double
            {
                computeWeightedBurnResponse_(aMidpoint, burnDuration, burnResponse);

                const ControlMatrix hessian = burnResponse.transpose() * burnResponse;
                const ControlVector gradient = burnResponse.transpose() * otherResidual;

                const ControlVector normalizedDeltaVelocity =
                    SolveSingleGroupLasso<ControlDimension>(hessian, gradient, parameters_.fuelWeight);

                const double cost = normalizedDeltaVelocity.dot(hessian * normalizedDeltaVelocity) +
                                    2.0 * gradient.dot(normalizedDeltaVelocity) +
                                    parameters_.fuelWeight * normalizedDeltaVelocity.norm();

                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestMidpoint = aMidpoint;
                    bestNormalizedDeltaVelocity = normalizedDeltaVelocity;
                }

                return cost;
            };

            evaluate(currentMidpoint);

            if (windowLength > 0.0)
            {
                const double candidateSpacing = windowLength / static_cast<double>(parameters_.burnStartCandidateCount);

                for (Size candidateIndex = 0; candidateIndex < parameters_.burnStartCandidateCount; ++candidateIndex)
                {
                    evaluate(windowStart + candidateIndex * candidateSpacing);
                }

                // Refine around the best candidate
                double lower = std::max(windowStart, bestMidpoint - candidateSpacing);
                double upper = std::min(windowStart + windowLength, bestMidpoint + candidateSpacing);

                double left = upper - goldenRatio * (upper - lower);
                double right = lower + goldenRatio * (upper - lower);
                double leftCost = evaluate(left);
                double rightCost = evaluate(right);

                for (Size iteration = 0; iteration < refinementIterationCount; ++iteration)
                {
                    if (leftCost < rightCost)
                    {
                        upper = right;
                        right = left;
                        rightCost = leftCost;
                        left = upper - goldenRatio * (upper - lower);
                        leftCost = evaluate(left);
                    }
                    else
                    {
                        lower = left;
                        left = right;
                        leftCost = rightCost;
                        right = lower + goldenRatio * (upper - lower);
                        rightCost = evaluate(right);
                    }
                }
            }

            if (std::abs(bestMidpoint - currentMidpoint) > midpointTolerance)
            {
                burnsMoved = true;
            }

            aSolution.burnMidpointOffsets[burnIndex] = bestMidpoint;
            aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * burnIndex) =
                bestNormalizedDeltaVelocity;

            if (keepFollowingBurnsInWindows_(burnIndex, aSolution))
            {
                residual =
                    computeWeightedResponseMatrix_(includeFirstBurn, aSolution) * aSolution.normalizedDeltaVelocities -
                    weightedTarget_;
            }
            else
            {
                residual = otherResidual +
                           computeWeightedBurnResponse_(bestMidpoint, burnDuration) * bestNormalizedDeltaVelocity;
            }
        }

        return burnsMoved;
    }

    // Sequential convex programming over the delta-v of all the burns, at fixed burn middles: re-linearize the burn
    // arcs about the previous burn durations. The plan is recomputed at every evaluation, so the iterations are damped:
    // undamped, they can cycle, and the burn durations of the last iteration then jump between nearby states. Returns
    // whether a burn moved by whole orbits to stay within its window.
    bool optimizeDeltaVelocities_(
        const bool& includeFirstBurn, const FirstBurnWindow& aFirstBurnWindow, Solution& aSolution
    ) const
    {
        static const Size maximumIterationCount = 20;
        static const Size undampedIterationCount = 2;
        static const double burnDurationTolerance = 1.0;  // [s]

        for (Size iteration = 0; iteration < maximumIterationCount; ++iteration)
        {
            const VectorXd previousBurnDurations = aSolution.burnDurations;

            ++aSolution.iterationCount;

            aSolution.normalizedDeltaVelocities = SolveGroupLasso<ControlDimension>(
                computeWeightedResponseMatrix_(includeFirstBurn, aSolution),
                weightedTarget_,
                parameters_.fuelWeight,
                aSolution.normalizedDeltaVelocities
            );

            const VectorXd burnDurations = computeBurnDurations_(aSolution);

            if ((burnDurations - previousBurnDurations).cwiseAbs().maxCoeff() < burnDurationTolerance)
            {
                break;
            }

            const bool isDamped = (iteration + 1) >= undampedIterationCount;

            for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
            {
                setBurnDuration_(
                    burnIndex,
                    isDamped ? (0.5 * (burnDurations[burnIndex] + previousBurnDurations[burnIndex]))
                             : burnDurations[burnIndex],
                    aFirstBurnWindow,
                    aSolution
                );
            }
        }

        // The burn durations follow the delta-v of the last sub-problem
        const VectorXd burnDurations = computeBurnDurations_(aSolution);

        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            setBurnDuration_(burnIndex, burnDurations[burnIndex], aFirstBurnWindow, aSolution);
        }

        return keepFollowingBurnsInWindows_(0, aSolution);
    }

    VectorXd computeBurnDurations_(const Solution& aSolution) const
    {
        VectorXd burnDurations(burnCount_);

        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            burnDurations[burnIndex] =
                aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * burnIndex).norm() *
                maximumBurnDuration_;
        }

        return burnDurations;
    }

    // Drop the burns that are too short to be executed, and evaluate the cost
    void finalize_(const bool& includeFirstBurn, const FirstBurnWindow& aFirstBurnWindow, Solution& aSolution) const
    {
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            if (aSolution.burnDurations[burnIndex] < parameters_.minimumBurnDuration.inSeconds())
            {
                aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * burnIndex).setZero();

                setBurnDuration_(burnIndex, 0.0, aFirstBurnWindow, aSolution);
            }
        }

        const VectorXd residual =
            computeWeightedResponseMatrix_(includeFirstBurn, aSolution) * aSolution.normalizedDeltaVelocities -
            weightedTarget_;

        double deltaVelocityTerm = 0.0;
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            deltaVelocityTerm +=
                aSolution.normalizedDeltaVelocities.segment<ControlDimension>(ControlDimension * burnIndex).norm();
        }

        aSolution.cost = residual.squaredNorm() + parameters_.fuelWeight * deltaVelocityTerm;
    }
};

// Effectivity of the first burn of a solution starting now: the fraction of the improvement over not burning of the
// best plan, whose first burn starts at its best start time within one orbit, that burning now achieves. When a
// threshold is provided, the optimization of the best plan stops as soon as the effectivity is known to be below it
// (and an effectivity of 0 is returned).
template <int ControlDimension>
double ComputeEffectivity(
    const BurnPlanner<ControlDimension>& aBurnPlanner, const BurnPlanSolution& aSolution, const double& aThreshold = 0.0
)
{
    const double noBurnCost = aBurnPlanner.solveWithoutFirstBurn(aSolution).cost;
    const double improvement = noBurnCost - aSolution.cost;

    if (!(improvement > 1e-12 * std::max(1.0, noBurnCost)))
    {
        return 0.0;
    }

    // Burning now is below the threshold as soon as a plan reaches this cost
    const double costBelowThreshold =
        (aThreshold > 0.0) ? (noBurnCost - improvement / aThreshold) : -std::numeric_limits<double>::infinity();

    const double bestCost = aBurnPlanner.solveFirstBurnStart(aSolution, costBelowThreshold).cost;

    if (bestCost < costBelowThreshold)
    {
        return 0.0;
    }

    return std::clamp(improvement / (noBurnCost - bestCost), 0.0, 1.0);
}

// Call a function with the burn planner from a state: its burns thrust radially only when the parameters allow it
template <typename Function>
auto WithBurnPlanner(
    const ROETargeting::Parameters& aParameters,
    const Vector6d& aMeanCOEVector,
    const Vector6d& aReferenceMeanCOEVector,
    const Vector6d& aTargetRelativeOrbitalElements,
    const Function& aFunction
)
{
    const Vector6d relativeOrbitalElements = ComputeRelativeOrbitalElements_(aMeanCOEVector, aReferenceMeanCOEVector);
    const double meanArgumentOfLatitude = aMeanCOEVector[4] + aMeanCOEVector[5];

    if (aParameters.allowRadialThrust)
    {
        return aFunction(BurnPlanner<3>(
            aParameters,
            aReferenceMeanCOEVector,
            relativeOrbitalElements,
            meanArgumentOfLatitude,
            aTargetRelativeOrbitalElements
        ));
    }

    return aFunction(BurnPlanner<2>(
        aParameters,
        aReferenceMeanCOEVector,
        relativeOrbitalElements,
        meanArgumentOfLatitude,
        aTargetRelativeOrbitalElements
    ));
}

}  // namespace

ROETargeting::Parameters::Parameters(
    const Duration& aBurnSpacing,
    const Duration& aMaximumBurnDuration,
    const Real& aThrustAcceleration,
    const Size& aHorizonBurnCount,
    const Vector6d& aToleranceVector,
    const Real& aFuelWeight,
    const Duration& aMinimumBurnDuration,
    const Real& aRelativeSemiMajorAxisRate,
    const Real& anEffectivityThreshold,
    const Size& aBurnStartCandidateCount,
    const bool& isRadialThrustAllowed
)
    : burnSpacing(aBurnSpacing),
      maximumBurnDuration(aMaximumBurnDuration),
      thrustAcceleration(aThrustAcceleration),
      horizonBurnCount(aHorizonBurnCount),
      toleranceVector(aToleranceVector),
      fuelWeight(aFuelWeight),
      minimumBurnDuration(aMinimumBurnDuration),
      relativeSemiMajorAxisRate(aRelativeSemiMajorAxisRate),
      effectivityThreshold(anEffectivityThreshold),
      burnStartCandidateCount(aBurnStartCandidateCount),
      allowRadialThrust(isRadialThrustAllowed)
{
    if (!burnSpacing.isDefined() || !maximumBurnDuration.isDefined() || !minimumBurnDuration.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Duration");
    }

    if (!maximumBurnDuration.isStrictlyPositive())
    {
        throw ostk::core::error::RuntimeError("Maximum burn duration must be strictly positive.");
    }

    if (burnSpacing < (2.0 * maximumBurnDuration))
    {
        throw ostk::core::error::RuntimeError("Burn spacing must be at least twice the maximum burn duration.");
    }

    if (!minimumBurnDuration.isPositive() || (minimumBurnDuration > maximumBurnDuration))
    {
        throw ostk::core::error::RuntimeError(
            "Minimum burn duration must be positive and lower than the maximum burn duration."
        );
    }

    if (!thrustAcceleration.isDefined() || !(thrustAcceleration > 0.0))
    {
        throw ostk::core::error::RuntimeError("Thrust acceleration must be strictly positive.");
    }

    if (horizonBurnCount == 0)
    {
        throw ostk::core::error::RuntimeError("Horizon burn count must be strictly positive.");
    }

    if (!(toleranceVector.array() > 0.0).all())
    {
        throw ostk::core::error::RuntimeError("Tolerances must be strictly positive.");
    }

    if (!fuelWeight.isDefined() || fuelWeight < 0.0)
    {
        throw ostk::core::error::RuntimeError("Fuel weight must be positive.");
    }

    if (!relativeSemiMajorAxisRate.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Relative semi-major axis rate");
    }

    if (!effectivityThreshold.isDefined() || effectivityThreshold < 0.0 || effectivityThreshold > 1.0)
    {
        throw ostk::core::error::RuntimeError("Effectivity threshold must be between 0 and 1.");
    }

    if (burnStartCandidateCount == 0)
    {
        throw ostk::core::error::RuntimeError("Burn start candidate count must be strictly positive.");
    }
}

ROETargeting::ROETargeting(
    const Trajectory& aReferenceTrajectory,
    const Parameters& aParameters,
    const Vector6d& aTargetRelativeOrbitalElements
)
    : GuidanceLaw("ROE Targeting"),
      referenceTrajectory_(aReferenceTrajectory),
      parameters_(aParameters),
      targetRelativeOrbitalElements_(aTargetRelativeOrbitalElements)
{
    if (!referenceTrajectory_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Reference trajectory");
    }
}

ROETargeting::~ROETargeting() {}

std::ostream& operator<<(std::ostream& anOutputStream, const ROETargeting& aGuidanceLaw)
{
    aGuidanceLaw.print(anOutputStream);

    return anOutputStream;
}

const Trajectory& ROETargeting::accessReferenceTrajectory() const
{
    return referenceTrajectory_;
}

ROETargeting::Parameters ROETargeting::getParameters() const
{
    return parameters_;
}

Vector6d ROETargeting::getTargetRelativeOrbitalElements() const
{
    return targetRelativeOrbitalElements_;
}

void ROETargeting::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "ROE Targeting") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Burn spacing:" << parameters_.burnSpacing.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum burn duration:" << parameters_.maximumBurnDuration.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Thrust acceleration [m/s^2]:" << parameters_.thrustAcceleration.toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Horizon burn count:" << parameters_.horizonBurnCount;
    ostk::core::utils::Print::Line(anOutputStream) << "Fuel weight:" << parameters_.fuelWeight.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Effectivity threshold:" << parameters_.effectivityThreshold.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Burn start candidate count:" << parameters_.burnStartCandidateCount;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Allow radial thrust:" << (parameters_.allowRadialThrust ? "True" : "False");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Vector3d ROETargeting::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const Vector6d meanCOEVector = this->computeMeanCOEVector_(State(
        anInstant,
        Position::Meters(aPositionCoordinates, outputFrameSPtr),
        Velocity::MetersPerSecond(aVelocityCoordinates, outputFrameSPtr)
    ));
    const Vector6d referenceMeanCOEVector = this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(anInstant));

    return WithBurnPlanner(
        parameters_,
        meanCOEVector,
        referenceMeanCOEVector,
        targetRelativeOrbitalElements_,
        [this, &aPositionCoordinates, &aVelocityCoordinates, &aThrustAcceleration](const auto& aBurnPlanner) -> Vector3d
        {
            const BurnPlanSolution solution = aBurnPlanner.solve(0.0);

            // No burn worth executing now: no need to compare with the rest of the orbit
            if (solution.burnDurations[0] <= 0.0)
            {
                return Vector3d::Zero();
            }

            if (ComputeEffectivity(aBurnPlanner, solution, parameters_.effectivityThreshold) <
                parameters_.effectivityThreshold)
            {
                return Vector3d::Zero();
            }

            return aThrustAcceleration * RTNToFrame(aPositionCoordinates, aVelocityCoordinates) *
                   aBurnPlanner.getNormalizedDeltaVelocity(solution, 0).normalized();
        }
    );
}

std::optional<GuidanceLaw::CommittedManeuver> ROETargeting::commitManeuverAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Shared<const Frame>& aFrameSPtr
) const
{
    const Plan plan = this->computePlan(State(
        anInstant,
        Position::Meters(aPositionCoordinates, aFrameSPtr),
        Velocity::MetersPerSecond(aVelocityCoordinates, aFrameSPtr)
    ));

    const Duration burnDuration = plan.burnIntervals.accessFirst().getDuration();

    if (!burnDuration.isStrictlyPositive())
    {
        return std::nullopt;
    }

    // The RTN frame of the plan is the QSW local orbital frame
    const Shared<const GuidanceLaw> constantThrustSPtr = std::make_shared<ConstantThrust>(LocalOrbitalFrameDirection(
        plan.deltaVelocities.accessFirst().normalized(), LocalOrbitalFrameFactory::QSW(aFrameSPtr)
    ));

    return CommittedManeuver {constantThrustSPtr, burnDuration};
}

Vector6d ROETargeting::computeRelativeOrbitalElements(const State& aState) const
{
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return ComputeRelativeOrbitalElements_(this->computeMeanCOEVector_(aState), referenceMeanCOEVector) *
           referenceMeanCOEVector[0];
}

ROETargeting::Plan ROETargeting::computePlan(const State& aState, const Duration& aFirstBurnDelay) const
{
    if (!aFirstBurnDelay.isDefined() || !aFirstBurnDelay.isPositive())
    {
        throw ostk::core::error::RuntimeError("First burn delay must be defined and positive.");
    }

    const Vector6d meanCOEVector = this->computeMeanCOEVector_(aState);
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return WithBurnPlanner(
        parameters_,
        meanCOEVector,
        referenceMeanCOEVector,
        targetRelativeOrbitalElements_,
        [&aState, &aFirstBurnDelay](const auto& aBurnPlanner) -> Plan
        {
            return aBurnPlanner.toPlan(aState.accessInstant(), aBurnPlanner.solve(aFirstBurnDelay.inSeconds()));
        }
    );
}

ROETargeting::Plan ROETargeting::computeBestPlan(const State& aState) const
{
    const Vector6d meanCOEVector = this->computeMeanCOEVector_(aState);
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return WithBurnPlanner(
        parameters_,
        meanCOEVector,
        referenceMeanCOEVector,
        targetRelativeOrbitalElements_,
        [&aState](const auto& aBurnPlanner) -> Plan
        {
            return aBurnPlanner.toPlan(
                aState.accessInstant(),
                aBurnPlanner.solveFirstBurnStart(aBurnPlanner.solve(0.0), -std::numeric_limits<double>::infinity())
            );
        }
    );
}

Real ROETargeting::computeEffectivity(const State& aState) const
{
    const Vector6d meanCOEVector = this->computeMeanCOEVector_(aState);
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return WithBurnPlanner(
        parameters_,
        meanCOEVector,
        referenceMeanCOEVector,
        targetRelativeOrbitalElements_,
        [](const auto& aBurnPlanner) -> Real
        {
            return ComputeEffectivity(aBurnPlanner, aBurnPlanner.solve(0.0));
        }
    );
}

Vector6d ROETargeting::ComputeRelativeOrbitalElements(const COE& aCOE, const COE& aReferenceCOE)
{
    return ComputeRelativeOrbitalElements_(MeanCOEVector(aCOE), MeanCOEVector(aReferenceCOE));
}

Matrix6d ROETargeting::ComputeStateTransitionMatrix(const COE& aReferenceCOE, const Duration& aDuration)
{
    return ComputeStateTransitionMatrix_(MeanCOEVector(aReferenceCOE), aDuration.inSeconds());
}

Matrix63d ROETargeting::ComputeControlInputMatrix(
    const Real& aSemiMajorAxis, const Real& aMeanArgumentOfLatitudeStart, const Real& aMeanArgumentOfLatitudeEnd
)
{
    static const double mu =
        EarthGravitationalModel::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());

    const double semiMajorAxis = aSemiMajorAxis;
    const double meanMotion = std::sqrt(mu / (semiMajorAxis * semiMajorAxis * semiMajorAxis));

    const double arc = aMeanArgumentOfLatitudeEnd - aMeanArgumentOfLatitudeStart;

    // Average of sin(u) and cos(u) over the arc
    double sinU;
    double cosU;

    if (std::abs(arc) < 1e-9)
    {
        sinU = std::sin(aMeanArgumentOfLatitudeStart);
        cosU = std::cos(aMeanArgumentOfLatitudeStart);
    }
    else
    {
        sinU = (std::cos(aMeanArgumentOfLatitudeStart) - std::cos(aMeanArgumentOfLatitudeEnd)) / arc;
        cosU = (std::sin(aMeanArgumentOfLatitudeEnd) - std::sin(aMeanArgumentOfLatitudeStart)) / arc;
    }

    Matrix63d controlInputMatrix;
    controlInputMatrix << 0.0, 2.0, 0.0, -2.0, 0.0, 0.0, sinU, 2.0 * cosU, 0.0, -cosU, 2.0 * sinU, 0.0, 0.0, 0.0, cosU,
        0.0, 0.0, sinU;

    return controlInputMatrix / (meanMotion * semiMajorAxis);
}

Vector6d ROETargeting::computeMeanCOEVector_(const State& aState) const
{
    const State state = aState.inFrame(Frame::GCRF());

    return MeanCOEVector(BrouwerLyddaneMeanLong::Cartesian(
        {state.getPosition(), state.getVelocity()}, EarthGravitationalModel::EGM2008.gravitationalParameter_
    ));
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
