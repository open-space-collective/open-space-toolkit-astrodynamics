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
const Duration Maneuver::MinimumRecommendedDuration = Duration::Seconds(30.0);
const Duration Maneuver::MaximumRecommendedInterpolationInterval = Duration::Minutes(2.0);

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
    if (this->instants_.isEmpty() || this->accelerationProfileDefaultFrame_.isEmpty() ||
        this->massFlowRateProfile_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No instants or accompanying accelerations/mass flow rates provided.");
    }

    if (instants_.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("At least two instants are required to define a maneuver.");
    }

    if (instants_.getSize() != accelerationProfileDefaultFrame_.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Acceleration profile must have the same number of elements as the number of instants."
        );
    }

    if (instants_.getSize() != massFlowRateProfile_.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Mass flow rate profile must have the same number of elements as the number of instants."
        );
    }

    const Duration maneuverDuration = instants_.accessLast() - instants_.accessFirst();
    Duration largestInterval = Duration::Zero();

    for (Size k = 0; k < instants_.getSize() - 1; ++k)
    {
        if (instants_[k] >= instants_[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted or Duplicate Instant Array");
        }

        largestInterval = std::max(largestInterval, instants_[k + 1] - instants_[k]);
    }

    if (largestInterval > Maneuver::MaximumRecommendedInterpolationInterval)
    {
        std::cout << "WARNING: Some intervals between the instants defined for this Maneuver are larger than the "
                     "maximum recommended interpolation interval of "
                  << Maneuver::MaximumRecommendedInterpolationInterval.inSeconds() << " seconds." << std::endl;
    }

    if (maneuverDuration < Maneuver::MinimumRecommendedDuration)
    {
        std::cout
            << "WARNING: maneuver duration is less than " << Maneuver::MinimumRecommendedDuration.inSeconds()
            << " seconds, it may be 'skipped over' and not taken into "
               "account during numerical propagation if the integrator's timestep is longer than the maneuver duration."
            << std::endl;
    }

    // Ensure that the accelerations provided have strictly positive magnitudes
    if (std::any_of(
            accelerationProfileDefaultFrame_.begin(),
            accelerationProfileDefaultFrame_.end(),
            [](const Vector3d& anAcceleration)
            {
                return anAcceleration.norm() <= 0.0;
            }
        ))
    {
        throw ostk::core::error::RuntimeError("Acceleration profile must have strictly positive magnitudes.");
    }

    // Ensure that mass flow rate profile is expressed in strictly negative numbers
    if (std::any_of(
            massFlowRateProfile_.begin(),
            massFlowRateProfile_.end(),
            [](Real aMassFlowRate)
            {
                return aMassFlowRate >= 0.0;
            }
        ))
    {
        throw ostk::core::error::RuntimeError("Mass flow rate profile must have strictly negative values.");
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
    // Use simple forward trapezoidal rule to calculate the total delta-v
    Real totalDeltaV = 0.0;
    for (Size i = 0; i < instants_.getSize() - 1; i++)
    {
        const Real timeStep = (instants_[i + 1] - instants_[i]).inSeconds();

        const Real currentAccelerationMagnitude = accelerationProfileDefaultFrame_[i].norm();
        const Real nextAccelerationMagnitude = accelerationProfileDefaultFrame_[i + 1].norm();

        totalDeltaV += ((currentAccelerationMagnitude + nextAccelerationMagnitude) / 2.0) * timeStep;
    }

    return totalDeltaV;
}

Mass Maneuver::calculateDeltaMass() const
{
    // Use simple forward trapezoidal rule to calculate the total delta-mass
    Real totalDeltaMass = 0.0;
    for (Size i = 0; i < instants_.getSize() - 1; i++)
    {
        const Real timeStep = (instants_[i + 1] - instants_[i]).inSeconds();

        totalDeltaMass += ((-massFlowRateProfile_[i] + -massFlowRateProfile_[i + 1]) / 2.0) * timeStep;
    }

    return Mass::Kilograms(totalDeltaMass);
}

Real Maneuver::calculateAverageThrust(const Mass& anInitialSpacecraftMass) const
{
    Real currentMass = anInitialSpacecraftMass.inKilograms();
    Real weightedThrustSum = 0.0;

    for (Size i = 0; i < instants_.getSize() - 1; i++)
    {
        const Real currentIntervalDuration = (instants_[i + 1] - instants_[i]).inSeconds();

        currentMass += ((massFlowRateProfile_[i] + massFlowRateProfile_[i + 1]) / 2.0) * currentIntervalDuration;
        const Real currentThrust =
            (accelerationProfileDefaultFrame_[i].norm() + accelerationProfileDefaultFrame_[i + 1].norm()) / 2 *
            currentMass;
        weightedThrustSum += currentThrust * currentIntervalDuration;
    }

    return weightedThrustSum / (instants_.accessLast() - instants_.accessFirst()).inSeconds();
}

Real Maneuver::calculateAverageSpecificImpulse(const Mass& anInitialSpacecraftMass) const
{
    const Real averageSpecificImpulse =
        (this->calculateAverageThrust(anInitialSpacecraftMass) * this->getInterval().getDuration().inSeconds()) /
        (this->calculateDeltaMass().inKilograms() * EarthGravitationalModel::gravityConstant);

    return averageSpecificImpulse;
}

Shared<Tabulated> Maneuver::toTabulatedDynamics(
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

    return std::make_shared<Tabulated>(
        Tabulated(instants_, contributionProfile, writeCoordinateSubset, aFrameSPtr, anInterpolationType)
    );
}

void Maneuver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Maneuver") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Interval:" << this->getInterval().toString();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Total delta-v:" << this->calculateDeltaV().toString() << " [m/s]";
    ostk::core::utils::Print::Line(anOutputStream) << "Total mass consumed:" << this->calculateDeltaMass().toString();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Maneuver Maneuver::TabulatedDynamics(const Tabulated& aTabulatedDynamics)
{
    if (!aTabulatedDynamics.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated Dynamics");
    }

    const MatrixXd contributionProfile = aTabulatedDynamics.getContributionProfileFromCoordinateSubsets(
        {CartesianVelocity::Default(), CoordinateSubset::Mass()}
    );
    Array<Vector3d> accelerationProfile = Array<Vector3d>::Empty();
    Array<Real> massFlowRateProfile = Array<Real>::Empty();

    for (Size i = 0; i < aTabulatedDynamics.accessInstants().getSize(); i++)
    {
        accelerationProfile.add(
            Vector3d(contributionProfile(i, 0), contributionProfile(i, 1), contributionProfile(i, 2))
        );
        massFlowRateProfile.add(contributionProfile(i, 3));
    }

    return {
        aTabulatedDynamics.accessInstants(),
        accelerationProfile,
        aTabulatedDynamics.accessFrame(),
        massFlowRateProfile,
    };
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
    Array<Vector3d> accelerationProfileInDefaultFrame = Array<Vector3d>(instants_.getSize(), Vector3d::Zero());
    for (Size i = 0; i < instants_.getSize(); i++)
    {
        accelerationProfileInDefaultFrame[i] = aFrameSPtr->getTransformTo(Maneuver::DefaultAccelFrameSPtr, instants_[i])
                                                   .applyToVector(accelerationProfileDefaultFrame_[i]);
    }
    return accelerationProfileInDefaultFrame;
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
