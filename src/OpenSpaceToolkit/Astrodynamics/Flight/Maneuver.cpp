/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;

const Shared<const Frame> Maneuver::DefaultAccelFrameSPtr = Frame::GCRF();
const Duration Maneuver::MinimumRecommendedDuration = Duration::Seconds(30.0);
const Duration Maneuver::MaximumRecommendedInterpolationInterval = Duration::Minutes(2.0);
const Shared<const CoordinateSubset> Maneuver::DefaultAccelerationCoordinateSubsetSPtr =
    CartesianAcceleration::Default();

Maneuver::Maneuver(const Array<State>& aStateArray, const Array<Real>& aMassFlowRateProfile)
    : states_(aStateArray),
      massFlowRateProfile_(aMassFlowRateProfile)
{
    // Sanitize the inputs
    if (this->states_.isEmpty() || this->massFlowRateProfile_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No states or accompanying mass flow rates provided.");
    }

    if (states_.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("At least two states are required to define a maneuver.");
    }

    if (states_.getSize() != massFlowRateProfile_.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Mass flow rate profile must have the same number of elements as the number of states."
        );
    }

    if (!std::all_of(
            states_.begin(),
            states_.end(),
            [](const State& aState)
            {
                return aState.hasSubset(DefaultAccelerationCoordinateSubsetSPtr);
            }
        ))
    {
        throw ostk::core::error::RuntimeError("Acceleration coordinate subset not found in states.");
    }

    const Duration maneuverDuration = states_.accessLast().accessInstant() - states_.accessFirst().accessInstant();
    Duration largestInterval = Duration::Zero();

    for (Size k = 0; k < states_.getSize() - 1; ++k)
    {
        if (states_[k].accessInstant() >= states_[k + 1].accessInstant())
        {
            throw ostk::core::error::runtime::Wrong("Unsorted or Duplicate State Array");
        }

        largestInterval = std::max(largestInterval, states_[k + 1].accessInstant() - states_[k].accessInstant());
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

    // Ensure that mass flow rate profile is expressed in strictly negative numbers
    if (std::any_of(
            massFlowRateProfile_.begin(),
            massFlowRateProfile_.end(),
            [](const Real& aMassFlowRate)
            {
                return aMassFlowRate >= 0.0;
            }
        ))
    {
        throw ostk::core::error::RuntimeError("Mass flow rate profile must have strictly negative values.");
    }
}

bool Maneuver::operator==(const Maneuver& aManeuver) const
{
    return states_ == aManeuver.states_ && massFlowRateProfile_ == aManeuver.massFlowRateProfile_;
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

Array<State> Maneuver::getStates() const
{
    return states_;
}

Array<Vector3d> Maneuver::getAccelerationProfile(const Shared<const Frame>& aFrameSPtr) const
{
    return states_.map<Vector3d>(
        [this, aFrameSPtr](const State& aState) -> Vector3d
        {
            return aState.inFrame(aFrameSPtr).extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr);
        }
    );
}

Array<Real> Maneuver::getMassFlowRateProfile() const
{
    return massFlowRateProfile_;
}

Interval Maneuver::getInterval() const
{
    return Interval::Closed(states_.accessFirst().accessInstant(), states_.accessLast().accessInstant());
}

Real Maneuver::calculateDeltaV() const
{
    // Use simple forward trapezoidal rule to calculate the total delta-v
    Real totalDeltaV = 0.0;
    for (Size i = 0; i < states_.getSize() - 1; i++)
    {
        const State& currentState = states_[i];
        const State& nextState = states_[i + 1];

        const Real timeStep = (nextState.accessInstant() - currentState.accessInstant()).inSeconds();

        const Real currentAccelerationMagnitude =
            currentState.extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr).norm();
        const Real nextAccelerationMagnitude =
            nextState.extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr).norm();

        totalDeltaV += ((currentAccelerationMagnitude + nextAccelerationMagnitude) / 2.0) * timeStep;
    }

    return totalDeltaV;
}

Mass Maneuver::calculateDeltaMass() const
{
    // Use simple forward trapezoidal rule to calculate the total delta-mass
    Real totalDeltaMass = 0.0;
    for (Size i = 0; i < states_.getSize() - 1; i++)
    {
        const State& currentState = states_[i];
        const State& nextState = states_[i + 1];

        const Real& thisMassFlowRate = massFlowRateProfile_[i];
        const Real& nextMassFlowRate = massFlowRateProfile_[i + 1];

        const Real timeStep = (nextState.accessInstant() - currentState.accessInstant()).inSeconds();

        totalDeltaMass += (-(thisMassFlowRate + nextMassFlowRate) / 2.0) * timeStep;
    }

    return Mass::Kilograms(totalDeltaMass);
}

Real Maneuver::calculateAverageThrust(const Mass& anInitialSpacecraftMass) const
{
    Real currentMass = anInitialSpacecraftMass.inKilograms();
    Real weightedThrustSum = 0.0;

    for (Size i = 0; i < states_.getSize() - 1; i++)
    {
        const State& currentState = states_[i];
        const State& nextState = states_[i + 1];

        const Real& thisMassFlowRate = massFlowRateProfile_[i];
        const Real& nextMassFlowRate = massFlowRateProfile_[i + 1];

        const Real timeStep = (nextState.accessInstant() - currentState.accessInstant()).inSeconds();

        currentMass += ((thisMassFlowRate + nextMassFlowRate) / 2.0) * timeStep;

        const Real currentAccelerationMagnitude =
            currentState.extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr).norm();
        const Real nextAccelerationMagnitude =
            nextState.extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr).norm();

        const Real currentThrust = (currentAccelerationMagnitude + nextAccelerationMagnitude) / 2.0 * currentMass;

        weightedThrustSum += currentThrust * timeStep;
    }

    return weightedThrustSum /
           (states_.accessLast().accessInstant() - states_.accessFirst().accessInstant()).inSeconds();
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
    const Array<Vector3d> accelerationProfileCustomFrame = this->getAccelerationProfile(aFrameSPtr);

    MatrixXd contributionProfile(states_.getSize(), 4);
    for (Size i = 0; i < states_.getSize(); i++)
    {
        contributionProfile(i, 0) = accelerationProfileCustomFrame[i](0);
        contributionProfile(i, 1) = accelerationProfileCustomFrame[i](1);
        contributionProfile(i, 2) = accelerationProfileCustomFrame[i](2);
        contributionProfile(i, 3) = massFlowRateProfile_[i];
    }

    const Array<Shared<const CoordinateSubset>> writeCoordinateSubset = {
        CartesianVelocity::Default(), CoordinateSubset::Mass()
    };

    const Array<Instant> instants = states_.map<Instant>(
        [](const State& aState) -> Instant
        {
            return aState.accessInstant();
        }
    );

    return std::make_shared<Tabulated>(
        Tabulated(instants, contributionProfile, writeCoordinateSubset, aFrameSPtr, anInterpolationType)
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

// Maneuver Maneuver::TabulatedDynamics(const Tabulated& aTabulatedDynamics)
// {
//     if (!aTabulatedDynamics.isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("Tabulated Dynamics");
//     }

//     const MatrixXd contributionProfile = aTabulatedDynamics.getContributionProfileFromCoordinateSubsets(
//         {CartesianVelocity::Default(), CoordinateSubset::Mass()}
//     );
//     Array<Vector3d> accelerationProfile = Array<Vector3d>::Empty();
//     Array<Real> massFlowRateProfile = Array<Real>::Empty();

//     for (Size i = 0; i < aTabulatedDynamics.accessInstants().getSize(); i++)
//     {
//         accelerationProfile.add(
//             Vector3d(contributionProfile(i, 0), contributionProfile(i, 1), contributionProfile(i, 2))
//         );
//         massFlowRateProfile.add(contributionProfile(i, 3));
//     }

//     return {
//         aTabulatedDynamics.accessInstants(),
//         accelerationProfile,
//         aTabulatedDynamics.accessFrame(),
//         massFlowRateProfile,
//     };
// }

Maneuver Maneuver::ConstantMassFlowRateProfile(const Array<State>& aStateArray, const Real& aMassFlowRate)
{
    return {
        aStateArray,
        Array<Real>(aStateArray.getSize(), aMassFlowRate),
    };
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
