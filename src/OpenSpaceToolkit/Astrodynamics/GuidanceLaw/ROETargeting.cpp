/// Apache License 2.0

#include <algorithm>
#include <limits>

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

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Index;

using ostk::mathematics::object::MatrixXd;
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

// Propagate mean orbital elements [a, e, i, Ω, ω, M] with the secular J2 rates
Vector6d PropagateMeanCOEVector(const Vector6d& aMeanCOEVector, const J2Quantities& aJ2Quantities, const double& aTau)
{
    Vector6d meanCOEVector = aMeanCOEVector;

    meanCOEVector[3] += aJ2Quantities.raanRate * aTau;
    meanCOEVector[4] += aJ2Quantities.aopRate * aTau;
    meanCOEVector[5] += aJ2Quantities.meanAnomalyRate * aTau;

    return meanCOEVector;
}

Matrix6d ComputeStateTransitionMatrix_(const Vector6d& aMeanCOEVector, const double& aTau)
{
    const J2Quantities q = ComputeJ2Quantities(aMeanCOEVector);

    const double e = aMeanCOEVector[1];
    const double aopInitial = aMeanCOEVector[4];
    const double aopFinal = aopInitial + q.aopRate * aTau;

    const double exi = e * std::cos(aopInitial);
    const double eyi = e * std::sin(aopInitial);
    const double exf = e * std::cos(aopFinal);
    const double eyf = e * std::sin(aopFinal);

    const double cosRotation = std::cos(q.aopRate * aTau);
    const double sinRotation = std::sin(q.aopRate * aTau);

    const double kt = q.kappa * aTau;

    Matrix6d stm = Matrix6d::Identity();

    stm(1, 0) = -(1.5 * q.n + 3.5 * q.kappa * q.E * q.P) * aTau;
    stm(1, 2) = kt * exi * q.F * q.G * q.P;
    stm(1, 3) = kt * eyi * q.F * q.G * q.P;
    stm(1, 4) = -kt * q.F * q.S;

    stm(2, 0) = 3.5 * kt * eyf * q.Q;
    stm(2, 2) = cosRotation - 4.0 * kt * exi * eyf * q.G * q.Q;
    stm(2, 3) = -sinRotation - 4.0 * kt * eyi * eyf * q.G * q.Q;
    stm(2, 4) = 5.0 * kt * eyf * q.S;

    stm(3, 0) = -3.5 * kt * exf * q.Q;
    stm(3, 2) = sinRotation + 4.0 * kt * exi * exf * q.G * q.Q;
    stm(3, 3) = cosRotation + 4.0 * kt * eyi * exf * q.G * q.Q;
    stm(3, 4) = -5.0 * kt * exf * q.S;

    stm(5, 0) = 3.5 * kt * q.S;
    stm(5, 2) = -4.0 * kt * exi * q.G * q.S;
    stm(5, 3) = -4.0 * kt * eyi * q.G * q.S;
    stm(5, 4) = 2.0 * kt * q.T;

    return stm;
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
// where u_k are the consecutive 3-vectors of u, with the Alternating Direction Method of Multipliers. The returned
// solution is exactly sparse and feasible.
VectorXd SolveGroupLasso(const MatrixXd& A, const VectorXd& b, const double& aWeight, const VectorXd& aWarmStart)
{
    static const Size maximumIterationCount = 20000;
    static const double absoluteTolerance = 1e-7;
    static const double relativeTolerance = 1e-6;

    const Index size = A.cols();
    const Index groupCount = size / 3;

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
            const Vector3d v = u.segment<3>(3 * group) + y.segment<3>(3 * group);
            const double vNorm = v.norm();

            Vector3d zGroup =
                (vNorm > 0.0) ? Vector3d(std::max(0.0, 1.0 - aWeight / (rho * vNorm)) * v) : Vector3d(Vector3d::Zero());

            const double zGroupNorm = zGroup.norm();
            if (zGroupNorm > 1.0)
            {
                zGroup /= zGroupNorm;
            }

            z.segment<3>(3 * group) = zGroup;
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

// Burn planning from a planning instant. The prediction samples, the weights and the free motion are shared by all
// the first burn delays compared by the effectivity.
class BurnPlanner
{
   public:
    struct Solution
    {
        double firstBurnDelay;
        VectorXd burnDurations;
        VectorXd normalizedDeltaVelocities;
        double cost;
        Size iterationCount;
    };

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
          burnCount_(aParameters.horizonBurnCount),
          sampleCount_(2 * aParameters.horizonBurnCount),
          burnSpacing_(aParameters.burnSpacing.inSeconds()),
          maximumBurnDuration_(aParameters.maximumBurnDuration.inSeconds()),
          maximumDeltaVelocity_(aParameters.thrustAcceleration * aParameters.maximumBurnDuration.inSeconds())
    {
        const double semiMajorAxis = referenceMeanCOEVector_[0];

        // Samples halfway through and at the end of each burn spacing after the planning instant, common to all the
        // first burn delays
        sampleOffsets_.resize(sampleCount_);
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            sampleOffsets_[2 * burnIndex] = (burnIndex + 0.5) * burnSpacing_;
            sampleOffsets_[2 * burnIndex + 1] = (burnIndex + 1.0) * burnSpacing_;
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

    // Period of the mean argument of latitude of the reference
    double getMeanArgumentOfLatitudePeriod() const
    {
        return 2.0 * M_PI / (j2Quantities_.aopRate + j2Quantities_.meanAnomalyRate);
    }

    // Sequential convex programming: re-linearize the burn arcs about the previous burn durations. The plan is
    // recomputed at every evaluation, so the iterations are damped: undamped, they can cycle, and the burn durations
    // of the last iteration then jump between nearby states.
    Solution solve(const double& aFirstBurnDelay, const bool& includeFirstBurn) const
    {
        static const Size maximumIterationCount = 20;
        static const Size undampedIterationCount = 2;
        static const double burnDurationTolerance = 1.0;  // [s]

        Solution solution = {
            aFirstBurnDelay,
            VectorXd::Zero(burnCount_),
            VectorXd::Zero(3 * burnCount_),
            0.0,
            0,
        };

        while (solution.iterationCount < maximumIterationCount)
        {
            const VectorXd previousBurnDurations = solution.burnDurations;

            solveConvexSubProblem_(includeFirstBurn, solution);

            const double burnDurationChange = (solution.burnDurations - previousBurnDurations).cwiseAbs().maxCoeff();

            if (burnDurationChange < burnDurationTolerance)
            {
                break;
            }

            if (solution.iterationCount >= undampedIterationCount)
            {
                solution.burnDurations = 0.5 * (solution.burnDurations + previousBurnDurations);
            }
        }

        // The burn durations follow the delta-v of the last sub-problem
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            solution.burnDurations[burnIndex] =
                solution.normalizedDeltaVelocities.segment<3>(3 * burnIndex).norm() * maximumBurnDuration_;
        }

        finalize_(includeFirstBurn, solution);

        return solution;
    }

    // Single convex sub-problem, linearized about the burn durations of another solution and warm-started from it
    Solution solveOnce(const double& aFirstBurnDelay, const bool& includeFirstBurn, const Solution& aSolution) const
    {
        Solution solution = aSolution;
        solution.firstBurnDelay = aFirstBurnDelay;
        solution.iterationCount = 0;

        // Without the first burn, start from zero rather than from the first burn of the warm start
        if (!includeFirstBurn)
        {
            solution.burnDurations[0] = 0.0;
            solution.normalizedDeltaVelocities.segment<3>(0).setZero();
        }

        solveConvexSubProblem_(includeFirstBurn, solution);
        finalize_(includeFirstBurn, solution);

        return solution;
    }

    ROETargeting::Plan toPlan(const Instant& anEpoch, const Solution& aSolution) const
    {
        const double semiMajorAxis = referenceMeanCOEVector_[0];

        const VectorXd predictedResponse =
            computeResponseMatrix_(aSolution.firstBurnDelay, true, aSolution.burnDurations) *
            aSolution.normalizedDeltaVelocities;

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
            plan.deltaVelocities.add(
                aSolution.normalizedDeltaVelocities.segment<3>(3 * burnIndex) * maximumDeltaVelocity_
            );
        }

        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            plan.predictionInstants.add(anEpoch + Duration::Seconds(sampleOffsets_[sampleIndex]));
            plan.predictedRelativeOrbitalElements.add(
                (freeMotion_.col(sampleIndex) + predictedResponse.segment<6>(6 * sampleIndex)) * semiMajorAxis
            );
        }

        return plan;
    }

   private:
    const ROETargeting::Parameters& parameters_;
    const Vector6d referenceMeanCOEVector_;
    const Vector6d relativeOrbitalElements_;
    const double meanArgumentOfLatitude_;
    const J2Quantities j2Quantities_;
    const Index burnCount_;
    const Index sampleCount_;
    const double burnSpacing_;
    const double maximumBurnDuration_;
    const double maximumDeltaVelocity_;

    VectorXd sampleOffsets_;
    Vector6d weights_;
    MatrixXd freeMotion_;
    VectorXd weightedTarget_;

    double computeBurnStartOffset_(const Solution& aSolution, const Index& aBurnIndex) const
    {
        return aSolution.firstBurnDelay + aBurnIndex * burnSpacing_;
    }

    // Response at the samples to the normalized delta-v (delta-v / maximum delta-v) of a burn
    MatrixXd computeBurnResponse_(const double& aBurnStart, const double& aBurnDuration) const
    {
        const double meanArgumentOfLatitudeRate = j2Quantities_.aopRate + j2Quantities_.meanAnomalyRate;
        const double burnMidpoint = aBurnStart + 0.5 * aBurnDuration;

        const Matrix63d controlInputMatrix = ROETargeting::ComputeControlInputMatrix(
            referenceMeanCOEVector_[0],
            meanArgumentOfLatitude_ + meanArgumentOfLatitudeRate * aBurnStart,
            meanArgumentOfLatitude_ + meanArgumentOfLatitudeRate * (aBurnStart + aBurnDuration)
        );

        const Vector6d midpointReferenceMeanCOEVector =
            PropagateMeanCOEVector(referenceMeanCOEVector_, j2Quantities_, burnMidpoint);

        MatrixXd burnResponse = MatrixXd::Zero(6 * sampleCount_, 3);

        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            if (sampleOffsets_[sampleIndex] < burnMidpoint)
            {
                continue;
            }

            burnResponse.middleRows<6>(6 * sampleIndex) =
                ComputeStateTransitionMatrix_(
                    midpointReferenceMeanCOEVector, sampleOffsets_[sampleIndex] - burnMidpoint
                ) *
                controlInputMatrix * maximumDeltaVelocity_;
        }

        return burnResponse;
    }

    // Response at the samples to the normalized delta-v (delta-v / maximum delta-v) of each burn
    MatrixXd computeResponseMatrix_(
        const double& aFirstBurnDelay, const bool& includeFirstBurn, const VectorXd& aBurnDurationArray
    ) const
    {
        MatrixXd responseMatrix = MatrixXd::Zero(6 * sampleCount_, 3 * burnCount_);

        for (Index burnIndex = (includeFirstBurn ? 0 : 1); burnIndex < burnCount_; ++burnIndex)
        {
            responseMatrix.middleCols<3>(3 * burnIndex) =
                computeBurnResponse_(aFirstBurnDelay + burnIndex * burnSpacing_, aBurnDurationArray[burnIndex]);
        }

        return responseMatrix;
    }

    MatrixXd computeWeightedResponseMatrix_(
        const double& aFirstBurnDelay, const bool& includeFirstBurn, const VectorXd& aBurnDurationArray
    ) const
    {
        MatrixXd weightedResponseMatrix = computeResponseMatrix_(aFirstBurnDelay, includeFirstBurn, aBurnDurationArray);

        for (Index sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex)
        {
            weightedResponseMatrix.middleRows<6>(6 * sampleIndex) =
                weights_.asDiagonal() * weightedResponseMatrix.middleRows<6>(6 * sampleIndex);
        }

        return weightedResponseMatrix;
    }

    // Solve the convex sub-problem linearized about the current burn durations, and update them
    void solveConvexSubProblem_(const bool& includeFirstBurn, Solution& aSolution) const
    {
        ++aSolution.iterationCount;

        aSolution.normalizedDeltaVelocities = SolveGroupLasso(
            computeWeightedResponseMatrix_(aSolution.firstBurnDelay, includeFirstBurn, aSolution.burnDurations),
            weightedTarget_,
            parameters_.fuelWeight,
            aSolution.normalizedDeltaVelocities
        );

        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            aSolution.burnDurations[burnIndex] =
                aSolution.normalizedDeltaVelocities.segment<3>(3 * burnIndex).norm() * maximumBurnDuration_;
        }
    }

    // Drop the burns that are too short to be executed, and evaluate the cost
    void finalize_(const bool& includeFirstBurn, Solution& aSolution) const
    {
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            if (aSolution.burnDurations[burnIndex] < parameters_.minimumBurnDuration.inSeconds())
            {
                aSolution.burnDurations[burnIndex] = 0.0;
                aSolution.normalizedDeltaVelocities.segment<3>(3 * burnIndex).setZero();
            }
        }

        const VectorXd residual =
            computeWeightedResponseMatrix_(aSolution.firstBurnDelay, includeFirstBurn, aSolution.burnDurations) *
                aSolution.normalizedDeltaVelocities -
            weightedTarget_;

        double deltaVelocityTerm = 0.0;
        for (Index burnIndex = 0; burnIndex < burnCount_; ++burnIndex)
        {
            deltaVelocityTerm += aSolution.normalizedDeltaVelocities.segment<3>(3 * burnIndex).norm();
        }

        aSolution.cost = residual.squaredNorm() + parameters_.fuelWeight * deltaVelocityTerm;
    }
};

// Effectivity of the first burn of a solution starting now: the fraction of the best improvement over not burning,
// among the first burn start times spread over one orbit, that burning now achieves. When a threshold is provided,
// the comparison stops as soon as the effectivity is known to be below it (and an effectivity of 0 is returned).
double ComputeEffectivity(
    const BurnPlanner& aBurnPlanner,
    const BurnPlanner::Solution& aSolution,
    const Size& aBurnStartCandidateCount,
    const double& aThreshold = 0.0
)
{
    const double noBurnCost = aBurnPlanner.solveOnce(0.0, false, aSolution).cost;
    const double improvement = noBurnCost - aSolution.cost;

    if (!(improvement > 1e-12 * std::max(1.0, noBurnCost)))
    {
        return 0.0;
    }

    // Burning now is below the threshold as soon as another start time reaches this cost
    const double costBelowThreshold =
        (aThreshold > 0.0) ? (noBurnCost - improvement / aThreshold) : -std::numeric_limits<double>::infinity();

    double bestCost = aSolution.cost;

    const double candidateSpacing =
        aBurnPlanner.getMeanArgumentOfLatitudePeriod() / static_cast<double>(aBurnStartCandidateCount);

    for (Size candidateIndex = 1; candidateIndex < aBurnStartCandidateCount; ++candidateIndex)
    {
        bestCost = std::min(bestCost, aBurnPlanner.solveOnce(candidateIndex * candidateSpacing, true, aSolution).cost);

        if (bestCost < costBelowThreshold)
        {
            return 0.0;
        }
    }

    return std::clamp(improvement / (noBurnCost - bestCost), 0.0, 1.0);
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
    const Size& aBurnStartCandidateCount
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
      burnStartCandidateCount(aBurnStartCandidateCount)
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

    const BurnPlanner burnPlanner = {
        parameters_,
        referenceMeanCOEVector,
        ComputeRelativeOrbitalElements_(meanCOEVector, referenceMeanCOEVector),
        meanCOEVector[4] + meanCOEVector[5],
        targetRelativeOrbitalElements_,
    };

    const BurnPlanner::Solution solution = burnPlanner.solve(0.0, true);

    // No burn worth executing now: no need to compare with the rest of the orbit
    if (solution.burnDurations[0] <= 0.0)
    {
        return Vector3d::Zero();
    }

    if (ComputeEffectivity(
            burnPlanner, solution, parameters_.burnStartCandidateCount, parameters_.effectivityThreshold
        ) < parameters_.effectivityThreshold)
    {
        return Vector3d::Zero();
    }

    return aThrustAcceleration * RTNToFrame(aPositionCoordinates, aVelocityCoordinates) *
           solution.normalizedDeltaVelocities.segment<3>(0).normalized();
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

    const BurnPlanner burnPlanner = {
        parameters_,
        referenceMeanCOEVector,
        ComputeRelativeOrbitalElements_(meanCOEVector, referenceMeanCOEVector),
        meanCOEVector[4] + meanCOEVector[5],
        targetRelativeOrbitalElements_,
    };

    return burnPlanner.toPlan(aState.accessInstant(), burnPlanner.solve(aFirstBurnDelay.inSeconds(), true));
}

Real ROETargeting::computeEffectivity(const State& aState) const
{
    const Vector6d meanCOEVector = this->computeMeanCOEVector_(aState);
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    const BurnPlanner burnPlanner = {
        parameters_,
        referenceMeanCOEVector,
        ComputeRelativeOrbitalElements_(meanCOEVector, referenceMeanCOEVector),
        meanCOEVector[4] + meanCOEVector[5],
        targetRelativeOrbitalElements_,
    };

    return ComputeEffectivity(burnPlanner, burnPlanner.solve(0.0, true), parameters_.burnStartCandidateCount);
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
