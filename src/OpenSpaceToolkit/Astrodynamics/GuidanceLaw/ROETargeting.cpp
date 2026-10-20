/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>
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

}  // namespace

ROETargeting::Parameters::Parameters(
    const Instant& aBurnWindowEpoch,
    const Duration& aBurnWindowPeriod,
    const Duration& aMaximumBurnDuration,
    const Real& aThrustAcceleration,
    const Size& aHorizonWindowCount,
    const Vector6d& aToleranceVector,
    const Real& aFuelWeight,
    const Duration& aMinimumBurnDuration,
    const Real& aRelativeSemiMajorAxisRate
)
    : burnWindowEpoch(aBurnWindowEpoch),
      burnWindowPeriod(aBurnWindowPeriod),
      maximumBurnDuration(aMaximumBurnDuration),
      thrustAcceleration(aThrustAcceleration),
      horizonWindowCount(aHorizonWindowCount),
      toleranceVector(aToleranceVector),
      fuelWeight(aFuelWeight),
      minimumBurnDuration(aMinimumBurnDuration),
      relativeSemiMajorAxisRate(aRelativeSemiMajorAxisRate)
{
    if (!burnWindowEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Burn window epoch");
    }

    if (!burnWindowPeriod.isDefined() || !maximumBurnDuration.isDefined() || !minimumBurnDuration.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Duration");
    }

    if (!maximumBurnDuration.isStrictlyPositive())
    {
        throw ostk::core::error::RuntimeError("Maximum burn duration must be strictly positive.");
    }

    if (burnWindowPeriod < (2.0 * maximumBurnDuration))
    {
        throw ostk::core::error::RuntimeError("Burn window period must be at least twice the maximum burn duration.");
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

    if (horizonWindowCount == 0)
    {
        throw ostk::core::error::RuntimeError("Horizon window count must be strictly positive.");
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
}

ROETargeting::ROETargeting(
    const Trajectory& aReferenceTrajectory,
    const Parameters& aParameters,
    const Vector6d& aTargetRelativeOrbitalElements
)
    : GuidanceLaw("ROE Targeting"),
      referenceTrajectory_(aReferenceTrajectory),
      parameters_(aParameters),
      targetRelativeOrbitalElements_(aTargetRelativeOrbitalElements),
      cachedBurnWindowIndex_(Integer::Undefined()),
      cachedBurnInterval_(Interval::Undefined()),
      cachedBurnDirection_(Vector3d::Zero())
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

Interval ROETargeting::getBurnWindowAt(const Instant& anInstant) const
{
    Integer windowIndex = std::max(this->getBurnWindowIndexAt_(anInstant), Integer(0));

    Instant windowStart = parameters_.burnWindowEpoch + parameters_.burnWindowPeriod * double(windowIndex);

    if (anInstant > (windowStart + parameters_.maximumBurnDuration))
    {
        windowStart += parameters_.burnWindowPeriod;
    }

    return Interval::Closed(windowStart, windowStart + parameters_.maximumBurnDuration);
}

void ROETargeting::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "ROE Targeting") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Burn window epoch:" << parameters_.burnWindowEpoch.toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Burn window period:" << parameters_.burnWindowPeriod.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum burn duration:" << parameters_.maximumBurnDuration.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Thrust acceleration [m/s^2]:" << parameters_.thrustAcceleration.toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Horizon window count:" << parameters_.horizonWindowCount;
    ostk::core::utils::Print::Line(anOutputStream) << "Fuel weight:" << parameters_.fuelWeight.toString();

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
    const Integer windowIndex = this->getBurnWindowIndexAt_(anInstant);

    std::lock_guard<std::mutex> lock(burnCacheMutex_);

    // An evaluation before the held window means a new propagation: discard the held plan
    if (cachedBurnWindowIndex_.isDefined() && (windowIndex < cachedBurnWindowIndex_))
    {
        cachedBurnWindowIndex_ = Integer::Undefined();
    }

    if (windowIndex < 0)
    {
        return Vector3d::Zero();
    }

    const Instant windowStart = parameters_.burnWindowEpoch + parameters_.burnWindowPeriod * double(windowIndex);

    if (anInstant > (windowStart + parameters_.maximumBurnDuration))
    {
        return Vector3d::Zero();
    }

    if (!cachedBurnWindowIndex_.isDefined() || (cachedBurnWindowIndex_ != windowIndex))
    {
        const State state = {
            anInstant,
            Position::Meters(aPositionCoordinates, outputFrameSPtr),
            Velocity::MetersPerSecond(aVelocityCoordinates, outputFrameSPtr),
        };

        const Vector6d meanCOEVector = this->computeMeanCOEVector_(state);
        const Vector6d referenceMeanCOEVector = this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(anInstant));

        // Propagate back to the window start, assuming no thrust since then
        const double tau = (windowStart - anInstant).inSeconds();
        const J2Quantities j2Quantities = ComputeJ2Quantities(referenceMeanCOEVector);

        const Vector6d relativeOrbitalElements = ComputeStateTransitionMatrix_(referenceMeanCOEVector, tau) *
                                                 ComputeRelativeOrbitalElements_(meanCOEVector, referenceMeanCOEVector);

        const Vector6d windowStartReferenceMeanCOEVector =
            PropagateMeanCOEVector(referenceMeanCOEVector, j2Quantities, tau);

        const double meanArgumentOfLatitude =
            meanCOEVector[4] + meanCOEVector[5] + (j2Quantities.aopRate + j2Quantities.meanAnomalyRate) * tau;

        const Plan plan = this->computePlan_(
            windowStart, windowStartReferenceMeanCOEVector, relativeOrbitalElements, meanArgumentOfLatitude
        );

        const Vector3d deltaVelocity = plan.deltaVelocities.accessFirst();

        cachedBurnWindowIndex_ = windowIndex;
        cachedBurnInterval_ = plan.burnIntervals.accessFirst();
        cachedBurnDirection_ = deltaVelocity.isZero() ? Vector3d(Vector3d::Zero()) : deltaVelocity.normalized();
    }

    // The burn interval is closed, so that a propagation ending exactly at the burn end thrusts until its last stage
    if (cachedBurnDirection_.isZero() || !cachedBurnInterval_.contains(anInstant))
    {
        return Vector3d::Zero();
    }

    return aThrustAcceleration * RTNToFrame(aPositionCoordinates, aVelocityCoordinates) * cachedBurnDirection_;
}

Vector6d ROETargeting::computeRelativeOrbitalElements(const State& aState) const
{
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return ComputeRelativeOrbitalElements_(this->computeMeanCOEVector_(aState), referenceMeanCOEVector) *
           referenceMeanCOEVector[0];
}

ROETargeting::Plan ROETargeting::computePlan(const State& aState) const
{
    const Vector6d meanCOEVector = this->computeMeanCOEVector_(aState);
    const Vector6d referenceMeanCOEVector =
        this->computeMeanCOEVector_(referenceTrajectory_.getStateAt(aState.accessInstant()));

    return this->computePlan_(
        aState.accessInstant(),
        referenceMeanCOEVector,
        ComputeRelativeOrbitalElements_(meanCOEVector, referenceMeanCOEVector),
        meanCOEVector[4] + meanCOEVector[5]
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

Integer ROETargeting::getBurnWindowIndexAt_(const Instant& anInstant) const
{
    return Integer(static_cast<int>(
        std::floor((anInstant - parameters_.burnWindowEpoch).inSeconds() / parameters_.burnWindowPeriod.inSeconds())
    ));
}

Vector6d ROETargeting::computeMeanCOEVector_(const State& aState) const
{
    const State state = aState.inFrame(Frame::GCRF());

    return MeanCOEVector(BrouwerLyddaneMeanLong::Cartesian(
        {state.getPosition(), state.getVelocity()}, EarthGravitationalModel::EGM2008.gravitationalParameter_
    ));
}

ROETargeting::Plan ROETargeting::computePlan_(
    const Instant& anEpoch,
    const Vector6d& aReferenceMeanCOEVector,
    const Vector6d& aRelativeOrbitalElements,
    const Real& aMeanArgumentOfLatitude
) const
{
    const Size windowCount = parameters_.horizonWindowCount;
    const Size sampleCount = 2 * windowCount;

    const Vector6d& referenceMeanCOEVector = aReferenceMeanCOEVector;
    const J2Quantities j2Quantities = ComputeJ2Quantities(referenceMeanCOEVector);

    const double semiMajorAxis = referenceMeanCOEVector[0];
    const double meanArgumentOfLatitudeRate = j2Quantities.aopRate + j2Quantities.meanAnomalyRate;
    const double period = parameters_.burnWindowPeriod.inSeconds();
    const double maximumBurnDuration = parameters_.maximumBurnDuration.inSeconds();
    const double maximumDeltaVelocity = parameters_.thrustAcceleration * maximumBurnDuration;
    const double relativeSemiMajorAxisRate = parameters_.relativeSemiMajorAxisRate / semiMajorAxis;

    // First window starting at or after the epoch
    const double epochWindowPosition =
        (anEpoch - parameters_.burnWindowEpoch).inSeconds() / parameters_.burnWindowPeriod.inSeconds();
    const Integer firstWindowIndex = std::max(0, static_cast<int>(std::ceil(epochWindowPosition - 1e-9)));
    const Instant firstWindowStart =
        parameters_.burnWindowEpoch + parameters_.burnWindowPeriod * double(firstWindowIndex);
    const double firstWindowOffset = std::max(0.0, double((firstWindowStart - anEpoch).inSeconds()));

    VectorXd windowOffsets(windowCount);
    VectorXd sampleOffsets(sampleCount);
    for (Index windowIndex = 0; windowIndex < windowCount; ++windowIndex)
    {
        windowOffsets[windowIndex] = firstWindowOffset + windowIndex * period;
        sampleOffsets[2 * windowIndex] = windowOffsets[windowIndex] + 0.5 * period;
        sampleOffsets[2 * windowIndex + 1] = windowOffsets[windowIndex] + period;
    }

    // Weights: tolerance-normalized errors, averaged over the samples
    const Vector6d weights =
        (semiMajorAxis / parameters_.toleranceVector.array()).matrix() / std::sqrt(static_cast<double>(sampleCount));

    const Vector6d targetRelativeOrbitalElements = targetRelativeOrbitalElements_ / semiMajorAxis;

    // Free motion at the samples
    MatrixXd freeMotion(6, sampleCount);
    for (Index sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        freeMotion.col(sampleIndex) =
            ComputeStateTransitionMatrix_(referenceMeanCOEVector, sampleOffsets[sampleIndex]) *
                aRelativeOrbitalElements +
            ComputeSemiMajorAxisRateEffect(referenceMeanCOEVector, sampleOffsets[sampleIndex]) *
                relativeSemiMajorAxisRate;
    }

    // Response at the samples to the normalized delta-v (delta-v / maximum delta-v) of each window
    const auto computeResponseMatrix = [&](const VectorXd& aBurnDurationArray) -> MatrixXd
    {
        MatrixXd responseMatrix = MatrixXd::Zero(6 * sampleCount, 3 * windowCount);

        for (Index windowIndex = 0; windowIndex < windowCount; ++windowIndex)
        {
            const double burnStart = windowOffsets[windowIndex];
            const double burnDuration = aBurnDurationArray[windowIndex];
            const double burnMidpoint = burnStart + 0.5 * burnDuration;

            const Matrix63d controlInputMatrix = ROETargeting::ComputeControlInputMatrix(
                semiMajorAxis,
                aMeanArgumentOfLatitude + meanArgumentOfLatitudeRate * burnStart,
                aMeanArgumentOfLatitude + meanArgumentOfLatitudeRate * (burnStart + burnDuration)
            );

            const Vector6d midpointReferenceMeanCOEVector =
                PropagateMeanCOEVector(referenceMeanCOEVector, j2Quantities, burnMidpoint);

            for (Index sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
            {
                if (sampleOffsets[sampleIndex] < burnMidpoint)
                {
                    continue;
                }

                responseMatrix.block<6, 3>(6 * sampleIndex, 3 * windowIndex) =
                    ComputeStateTransitionMatrix_(
                        midpointReferenceMeanCOEVector, sampleOffsets[sampleIndex] - burnMidpoint
                    ) *
                    controlInputMatrix * maximumDeltaVelocity;
            }
        }

        return responseMatrix;
    };

    // Weighted least-squares form of the tracking error
    VectorXd weightedTarget(6 * sampleCount);
    for (Index sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        weightedTarget.segment<6>(6 * sampleIndex) =
            weights.cwiseProduct(targetRelativeOrbitalElements - freeMotion.col(sampleIndex));
    }

    // Sequential convex programming: re-linearize the burn arcs about the previous burn durations
    static const Size maximumIterationCount = 10;
    static const double burnDurationTolerance = 1.0;  // [s]

    VectorXd burnDurations = VectorXd::Zero(windowCount);
    VectorXd normalizedDeltaVelocities = VectorXd::Zero(3 * windowCount);
    Size iterationCount = 0;

    while (iterationCount < maximumIterationCount)
    {
        ++iterationCount;

        MatrixXd weightedResponseMatrix = computeResponseMatrix(burnDurations);
        for (Index sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
        {
            weightedResponseMatrix.middleRows<6>(6 * sampleIndex) =
                weights.asDiagonal() * weightedResponseMatrix.middleRows<6>(6 * sampleIndex);
        }

        normalizedDeltaVelocities =
            SolveGroupLasso(weightedResponseMatrix, weightedTarget, parameters_.fuelWeight, normalizedDeltaVelocities);

        VectorXd updatedBurnDurations(windowCount);
        for (Index windowIndex = 0; windowIndex < windowCount; ++windowIndex)
        {
            updatedBurnDurations[windowIndex] =
                normalizedDeltaVelocities.segment<3>(3 * windowIndex).norm() * maximumBurnDuration;
        }

        const double burnDurationChange = (updatedBurnDurations - burnDurations).cwiseAbs().maxCoeff();

        burnDurations = updatedBurnDurations;

        if (burnDurationChange < burnDurationTolerance)
        {
            break;
        }
    }

    // Drop the burns that are too short to be executed
    for (Index windowIndex = 0; windowIndex < windowCount; ++windowIndex)
    {
        if (burnDurations[windowIndex] < parameters_.minimumBurnDuration.inSeconds())
        {
            burnDurations[windowIndex] = 0.0;
            normalizedDeltaVelocities.segment<3>(3 * windowIndex).setZero();
        }
    }

    const VectorXd predictedResponse = computeResponseMatrix(burnDurations) * normalizedDeltaVelocities;

    Plan plan = {
        anEpoch,
        semiMajorAxis,
        aRelativeOrbitalElements * semiMajorAxis,
        Array<Interval>::Empty(),
        Array<Vector3d>::Empty(),
        Array<Instant>::Empty(),
        Array<Vector6d>::Empty(),
        iterationCount,
    };

    for (Index windowIndex = 0; windowIndex < windowCount; ++windowIndex)
    {
        const Instant burnStart = anEpoch + Duration::Seconds(windowOffsets[windowIndex]);

        plan.burnIntervals.add(Interval::Closed(burnStart, burnStart + Duration::Seconds(burnDurations[windowIndex])));
        plan.deltaVelocities.add(normalizedDeltaVelocities.segment<3>(3 * windowIndex) * maximumDeltaVelocity);
    }

    for (Index sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        plan.predictionInstants.add(anEpoch + Duration::Seconds(sampleOffsets[sampleIndex]));
        plan.predictedRelativeOrbitalElements.add(
            (freeMotion.col(sampleIndex) + predictedResponse.segment<6>(6 * sampleIndex)) * semiMajorAxis
        );
    }

    return plan;
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
