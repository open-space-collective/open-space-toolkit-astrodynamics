/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

const Shared<const Frame> Maneuver::DefaultAccelFrameSPtr = Frame::GCRF();
const Duration Maneuver::MinimumRecommendedDuration = Duration::Seconds(30.0);
const Duration Maneuver::MaximumRecommendedInterpolationInterval = Duration::Minutes(2.0);
const Array<Shared<const CoordinateSubset>> Maneuver::RequiredCoordinateSubsets = {
    CartesianPosition::Default(),
    CartesianVelocity::Default(),
    CartesianAcceleration::Default(),
    CoordinateSubset::MassFlowRate(),
};
const Shared<const CoordinateSubset> Maneuver::DefaultAccelerationCoordinateSubsetSPtr = RequiredCoordinateSubsets[2];

Maneuver::Maneuver(const Array<State>& aStateArray)
    : states_(aStateArray)
{
    // Sanitize the inputs
    if (this->states_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No states provided.");
    }

    if (states_.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("At least two states are required to define a maneuver.");
    }

    for (const auto& coordinateSubset : RequiredCoordinateSubsets)
    {
        if (!std::all_of(
                states_.begin(),
                states_.end(),
                [&coordinateSubset](const State& aState)
                {
                    return aState.hasSubset(coordinateSubset);
                }
            ))
        {
            throw ostk::core::error::RuntimeError(String::Format("{} not found in states.", coordinateSubset->getName())
            );
        }
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
            states_.begin(),
            states_.end(),
            [](const State& aState)
            {
                return aState.extractCoordinate(RequiredCoordinateSubsets[3])[0] >= 0.0;
            }
        ))
    {
        throw ostk::core::error::RuntimeError("Mass flow rate profile must have strictly negative values.");
    }
}

bool Maneuver::operator==(const Maneuver& aManeuver) const
{
    return states_ == aManeuver.states_;
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

        const Real currentMassFlowRate = currentState.extractCoordinate(RequiredCoordinateSubsets[3])[0];
        const Real nextMassFlowRate = nextState.extractCoordinate(RequiredCoordinateSubsets[3])[0];

        const Real timeStep = (nextState.accessInstant() - currentState.accessInstant()).inSeconds();

        totalDeltaMass += (-(currentMassFlowRate + nextMassFlowRate) / 2.0) * timeStep;
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

        const Real currentMassFlowRate = currentState.extractCoordinate(RequiredCoordinateSubsets[3])[0];
        const Real nextMassFlowRate = nextState.extractCoordinate(RequiredCoordinateSubsets[3])[0];

        const Real timeStep = (nextState.accessInstant() - currentState.accessInstant()).inSeconds();

        currentMass += ((currentMassFlowRate + nextMassFlowRate) / 2.0) * timeStep;

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
    const Array<Vector3d> accelerationProfileCustomFrame = states_.map<Vector3d>(
        [this, aFrameSPtr](const State& aState) -> Vector3d
        {
            return aState.inFrame(aFrameSPtr).extractCoordinate(DefaultAccelerationCoordinateSubsetSPtr);
        }
    );

    const Array<Real> massFlowRateProfile = states_.map<Real>(
        [this](const State& aState) -> Real
        {
            return aState.extractCoordinate(RequiredCoordinateSubsets[3])[0];
        }
    );

    MatrixXd contributionProfile(states_.getSize(), 4);
    for (Size i = 0; i < states_.getSize(); i++)
    {
        contributionProfile(i, 0) = accelerationProfileCustomFrame[i](0);
        contributionProfile(i, 1) = accelerationProfileCustomFrame[i](1);
        contributionProfile(i, 2) = accelerationProfileCustomFrame[i](2);
        contributionProfile(i, 3) = massFlowRateProfile[i];
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

Maneuver Maneuver::ConstantMassFlowRateProfile(const Array<State>& aStateArray, const Real& aMassFlowRate)
{
    Array<State> maneuverStates = Array<State>::Empty();
    maneuverStates.reserve(aStateArray.getSize());

    static const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CartesianAcceleration::Default(),
        CoordinateSubset::MassFlowRate(),
    };

    for (const auto& state : aStateArray)
    {
        VectorXd coordinates(10);
        coordinates << state.extractCoordinates({coordinateSubsets[0], coordinateSubsets[1], coordinateSubsets[2]}),
            aMassFlowRate;

        const State maneuverState = {
            state.accessInstant(),
            coordinates,
            state.accessFrame(),
            coordinateSubsets,
        };

        maneuverStates.add(maneuverState);
    }

    return {maneuverStates};
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
