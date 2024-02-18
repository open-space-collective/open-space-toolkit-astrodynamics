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

bool Maneuver::operator==(const Maneuver& aManeuver) const
{
    return instants_ == aManeuver.instants_ &&
           accelerationProfileDefaultFrame_ == aManeuver.accelerationProfileDefaultFrame_ &&
           massFlowRateProfile_ == aManeuver.massFlowRateProfile_;
}

bool Maneuver::operator!=(const Maneuver& aManeuver) const
{
    return !(*this == aManeuver);
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

Maneuver Maneuver::ConstantMassFlowRateProfile(
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
        // TBI: fine a way to check and vet whether or not the frame is local, or quasi-inertial
        accelerationProfileInCustomFrame[i] = aFrameSPtr->getTransformTo(Maneuver::DefaultAccelFrameSPtr, instants_[i])
                                                  .applyToVector(accelerationProfileDefaultFrame_[i]);
    }
    return accelerationProfileInCustomFrame;
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
