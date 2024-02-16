/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

const Shared<const Frame> Maneuver::DefaultAccelFrameSPtr = Frame::GCRF();

Maneuver::Maneuver(
    const Array<Instant>& anInstantArray,
    const Array<Vector3d>& anAccelerationProfile,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Real>& aMassFlowRateProfile
)
    : instants_(anInstantArray),
      accelerationProfileDefaultFrame_(anAccelerationProfile),
      massFlowRateProfile_(aMassFlowRateProfile)
{
    // Sanitize the inputs
    if (instants_.getSize() != accelerationProfileDefaultFrame_.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Acceleration profile must have the same number of elements as the number of instants."
        );
    }

    if (this->instants_.isEmpty() || this->accelerationProfileDefaultFrame_.isEmpty() ||
        this->massFlowRateProfile_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No instants or accompanying accelerations/mass flow rates provided.");
    }

    for (Size k = 0; k < instants_.getSize() - 1; ++k)
    {
        if (instants_[k] > instants_[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array");
        }
    }

    // Convert to the default frame if necessary
    accelerationProfileDefaultFrame_ = this->convertAccelerationProfileFrame(aFrameSPtr);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Maneuver& aManeuver)
{
    aManeuver.print(anOutputStream);

    return anOutputStream;
}

bool Maneuver::isDefined() const
{
    return true;
}

Array<Instant> Maneuver::getInstants() const
{
    return instants_;
}

Array<Vector3d> Maneuver::getAccelerationProfile(const Shared<const Frame>& aFrameSPtr) const
{
    return this->convertAccelerationProfileFrame(aFrameSPtr);
}

Array<Real> Maneuver::getMassFlowRateProfile() const
{
    return massFlowRateProfile_;
}

Interval Maneuver::getInterval() const
{
    return Interval::Closed(instants_.accessFirst(), instants_.accessLast());
}

Real Maneuver::calculateDeltaV() const
{
    Real weightedAccelerationMagnitudeSum = 0.0;
    Real totalTime = 0.0;

    for (Size i = 0; i < instants_.getSize(); i++)
    {
        const Real timeStep = (instants_[i] - instants_.accessFirst()).inSeconds();
        weightedAccelerationMagnitudeSum += timeStep * accelerationProfileDefaultFrame_[i].norm();
        totalTime += timeStep;
    }

    return weightedAccelerationMagnitudeSum / totalTime;
}

Mass Maneuver::calculateDeltaMass() const
{
    Real weightedMassSum = 0.0;

    for (Size i = 0; i < instants_.getSize(); i++)
    {
        const Real timeStep = (instants_[i] - instants_.accessFirst()).inSeconds();

        weightedMassSum += timeStep * massFlowRateProfile_[i];
    }

    return {weightedMassSum, Mass::Unit::Kilogram};
}

Real Maneuver::calculateAverageThrust(const Mass& anInitialSpacecraftMass) const
{
    Real weightedThrustSum = 0.0;
    Real weightedMassSum = anInitialSpacecraftMass.inKilograms();
    Real totalTime = 0.0;

    for (Size i = 0; i < instants_.getSize(); i++)
    {
        const Real timeStep = (instants_[i] - instants_.accessFirst()).inSeconds();
        weightedMassSum -= massFlowRateProfile_[i];
        weightedThrustSum += timeStep * accelerationProfileDefaultFrame_[i].norm() * weightedMassSum;
        totalTime += timeStep;
    }

    return weightedThrustSum / totalTime;
}

Real Maneuver::calculateAverageSpecificImpulse(const Mass& anInitialSpacecraftMass) const
{
    const Real averageSpecificImpulse =
        (this->calculateAverageThrust(anInitialSpacecraftMass) * this->getInterval().getDuration().inSeconds()) /
        (this->calculateDeltaMass().inKilograms() * EarthGravitationalModel::gravityConstant);

    return averageSpecificImpulse;
}

Vector3d Maneuver::calculateAccelerationAt(
    const Instant& anInstant, const Shared<const Frame>& aFrameSPtr, const Interpolator::Type& anInterpolationType
) const
{
    if (anInstant < instants_.accessFirst() || anInstant > instants_.accessLast())
    {
        throw ostk::core::error::RuntimeError("Instant must be within the range of the maneuver.");
    }

    return this->interpolateAccelerations(Array<Instant>(1, anInstant), aFrameSPtr, anInterpolationType)[0];
}

Array<Vector3d> Maneuver::calculateAccelerationsAt(
    const Array<Instant>& anInstantArray,
    const Shared<const Frame>& aFrameSPtr,
    const Interpolator::Type& anInterpolationType
) const
{
    for (const Instant& instant : anInstantArray)
    {
        if (instant < instants_.accessFirst() || instant > instants_.accessLast())
        {
            throw ostk::core::error::RuntimeError("Instants must be within the range of the maneuver.");
        }
    }

    return this->interpolateAccelerations(anInstantArray, aFrameSPtr, anInterpolationType);
}

Real Maneuver::calculateMassFlowRateAt(const Instant& anInstant, const Interpolator::Type& anInterpolationType) const
{
    if (anInstant < instants_.accessFirst() || anInstant > instants_.accessLast())
    {
        throw ostk::core::error::RuntimeError("Instant must be within the range of the maneuver.");
    }

    return this->interpolateMassFlowRates(Array<Instant>(1, anInstant), anInterpolationType)[0];
}

Array<Real> Maneuver::calculateMassFlowRatesAt(
    const Array<Instant>& anInstantArray, const Interpolator::Type& anInterpolationType
) const
{
    for (const Instant& instant : anInstantArray)
    {
        if (instant < instants_.accessFirst() || instant > instants_.accessLast())
        {
            throw ostk::core::error::RuntimeError("Instants must be within the range of the maneuver.");
        }
    }

    return this->interpolateMassFlowRates(anInstantArray, anInterpolationType);
}

Shared<TabulatedDynamics> Maneuver::toTabulatedDynamics(
    const Shared<const Frame>& aFrameSPtr, const Interpolator::Type& anInterpolationType
) const
{
    const Array<Vector3d> accelerationProfileCustomFrame = this->convertAccelerationProfileFrame(aFrameSPtr);

    MatrixXd contributionProfile(instants_.getSize(), 4);
    for (Size i = 0; i < instants_.getSize(); i++)
    {
        contributionProfile(i, 0) = accelerationProfileCustomFrame[i](0);
        contributionProfile(i, 1) = accelerationProfileCustomFrame[i](1);
        contributionProfile(i, 2) = accelerationProfileCustomFrame[i](2);
        contributionProfile(i, 3) = massFlowRateProfile_[i];
    }

    const Array<Shared<const CoordinateSubset>> writeCoordinateSubset = {
        CartesianVelocity::Default(), CoordinateSubset::Mass()
    };

    return std::make_shared<TabulatedDynamics>(
        TabulatedDynamics(instants_, contributionProfile, writeCoordinateSubset, aFrameSPtr, anInterpolationType)
    );
}

void Maneuver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Maneuver") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Interval:" << this->getInterval().toString();

    ostk::core::utils::Print::Line(anOutputStream) << "Total delta-v:" << this->calculateDeltaV().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Total mass consumed:" << this->calculateDeltaMass().toString();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Maneuver Maneuver::FromConstantMassFlowRateProfile(
    const Array<Instant>& anInstantArray,
    const Array<Vector3d>& anAccelerationProfile,
    const Shared<const Frame>& aFrameSPtr,
    const Real& aMassFlowRate
)
{
    return {anInstantArray, anAccelerationProfile, aFrameSPtr, Array<Real>(anInstantArray.getSize(), aMassFlowRate)};
}

Array<Vector3d> Maneuver::convertAccelerationProfileFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if (aFrameSPtr == Maneuver::DefaultAccelFrameSPtr)
    {
        return accelerationProfileDefaultFrame_;
    }

    // Convert to the desired frame if necessary
    Array<Vector3d> accelerationProfileInCustomFrame = Array<Vector3d>(instants_.getSize(), Vector3d::Zero());
    for (Size i = 0; i < instants_.getSize(); i++)
    {
        accelerationProfileInCustomFrame[i] = aFrameSPtr->getTransformTo(Maneuver::DefaultAccelFrameSPtr, instants_[i])
                                                  .applyToVector(accelerationProfileDefaultFrame_[i]);
    }
    return accelerationProfileInCustomFrame;
}

Array<Vector3d> Maneuver::interpolateAccelerations(
    const Array<Instant>& anInstantArray,
    const Shared<const Frame>& aFrameSPtr,
    const Interpolator::Type& anInterpolationType
) const
{
    const Array<Vector3d> accelerationProfileCustomFrame = this->convertAccelerationProfileFrame(aFrameSPtr);

    VectorXd maneuverTimestamps(instants_.getSize());
    MatrixXd contributionProfile(instants_.getSize(), 3);

    for (Size i = 0; i < instants_.getSize(); i++)
    {
        maneuverTimestamps(i) = (instants_[i] - instants_.accessFirst()).inSeconds();
        contributionProfile(i, 0) = accelerationProfileCustomFrame[i](0);
        contributionProfile(i, 1) = accelerationProfileCustomFrame[i](1);
        contributionProfile(i, 2) = accelerationProfileCustomFrame[i](2);
    }

    Array<Shared<const Interpolator>> interpolators = Array<Shared<const Interpolator>>::Empty();
    interpolators.reserve(contributionProfile.cols());

    for (Size i = 0; i < (Size)contributionProfile.cols(); i++)
    {
        interpolators.add(
            Interpolator::GenerateInterpolator(anInterpolationType, maneuverTimestamps, contributionProfile.col(i))
        );
    }

    Array<Vector3d> interpolatedAccelerations = Array<Vector3d>::Empty();
    interpolatedAccelerations.reserve(anInstantArray.getSize());

    for (Size i = 0; i < anInstantArray.getSize(); i++)
    {
        Vector3d interpolatedAcceleration = Vector3d::Zero();
        for (Size j = 0; j < interpolators.getSize(); j++)
        {
            interpolatedAcceleration(j) =
                interpolators[j]->evaluate((anInstantArray[i] - instants_.accessFirst()).inSeconds());
        }
        interpolatedAccelerations.add(interpolatedAcceleration);
    }

    return interpolatedAccelerations;
}

Array<Real> Maneuver::interpolateMassFlowRates(
    const Array<Instant>& anInstantArray, const Interpolator::Type& anInterpolationType
) const
{
    VectorXd maneuverTimestamps(instants_.getSize());
    VectorXd massFlowRates(instants_.getSize());

    for (Size i = 0; i < instants_.getSize(); i++)
    {
        maneuverTimestamps(i) = (instants_[i] - instants_.accessFirst()).inSeconds();
        massFlowRates(i) = massFlowRateProfile_[i];
    }

    const Shared<const Interpolator> interpolatorSPtr =
        Interpolator::GenerateInterpolator(anInterpolationType, maneuverTimestamps, massFlowRates);

    Array<Real> interpolatedMassFlowRates = Array<Real>::Empty();
    interpolatedMassFlowRates.reserve(anInstantArray.getSize());

    for (Size i = 0; i < anInstantArray.getSize(); i++)
    {
        interpolatedMassFlowRates.add(
            interpolatorSPtr->evaluate((anInstantArray[i] - instants_.accessFirst()).inSeconds())
        );
    }

    return interpolatedMassFlowRates;
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
