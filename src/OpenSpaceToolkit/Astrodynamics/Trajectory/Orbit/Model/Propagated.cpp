/// Apache License 2.0

#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::core::type::Size;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Interval;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

using ostk::astrodynamics::trajectory::orbit::model::Tabulated;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

Propagated::Propagated(
    const Propagator& aPropagator, const State& aState, int aRevolutionNumber, const Array<Maneuver>& aManeuverArray
)
    : Model(),
      propagator_(aPropagator),
      cachedStateArray_(1, aState),
      initialRevolutionNumber_(aRevolutionNumber),
      maneuvers_(aManeuverArray)
{
    validateManeuvers();
}

Propagated::Propagated(
    const Propagator& aPropagator,
    const Array<State>& aCachedStateArray,
    int aRevolutionNumber,
    const Array<Maneuver>& aManeuverArray
)
    : Model(),
      propagator_(aPropagator),
      cachedStateArray_(aCachedStateArray),
      initialRevolutionNumber_(aRevolutionNumber),
      maneuvers_(aManeuverArray)
{
    if (cachedStateArray_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Cached state array is empty.");
    }

    sanitizeCachedArray();

    this->validateManeuvers();
}

Propagated* Propagated::clone() const
{
    return new Propagated(*this);
}

bool Propagated::operator==(const Propagated& aPropagatedModel) const
{
    return (cachedStateArray_ == aPropagatedModel.cachedStateArray_) && (propagator_ == aPropagatedModel.propagator_) &&
           (maneuvers_ == aPropagatedModel.maneuvers_);
}

bool Propagated::operator!=(const Propagated& aPropagatedModel) const
{
    return !((*this) == aPropagatedModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Propagated& aPropagatedModel)
{
    aPropagatedModel.print(anOutputStream);

    return anOutputStream;
}

Instant Propagated::getEpoch() const
{
    return cachedStateArray_[0].getInstant();
}

int Propagated::getRevolutionNumberAtEpoch() const
{
    return this->initialRevolutionNumber_;
}

State Propagated::calculateStateAt(const Instant& anInstant) const
{
    const Array<State> propagatedStateArray = this->calculateStatesAt(Array(1, anInstant));

    if (propagatedStateArray.getSize() != 1)
    {
        throw ostk::core::error::runtime::Wrong(
            "Propagated state array size", String::Format("Expected: 1, Got: {}", propagatedStateArray.getSize())
        );
    }

    return propagatedStateArray[0];
}

Array<State> Propagated::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    for (Size k = 0; k < anInstantArray.getSize() - 1; ++k)
    {
        if (anInstantArray[k] > anInstantArray[k + 1])
        {
            throw ostk::core::error::runtime::Wrong(
                "Unsorted Instant Array",
                String::Format(
                    "Index {}: {} > Index {}: {}",
                    k,
                    anInstantArray[k].toString(),
                    k + 1,
                    anInstantArray[k + 1].toString()
                )
            );
        }
    }

    // Builder for output states based on cached array
    StateBuilder outputStateBuilder = {this->cachedStateArray_.getFirst()};

    Array<State> allStates = Array<State>::Empty();

    // Maintain counter separately so as to only iterate once through instant array

    Size j = 0;

    // Propagate states for instants < first cached State

    Array<Instant> instants = Array<Instant>::Empty();
    for (; j < anInstantArray.getSize(); ++j)
    {
        if (anInstantArray[j] > this->cachedStateArray_.getFirst().getInstant())
        {
            break;
        }

        instants.add(anInstantArray[j]);
    }

    allStates.add(propagator_.calculateStatesAt(this->cachedStateArray_.getFirst(), instants, maneuvers_));

    // Propagate all instants between states

    for (Size i = 0; i < this->cachedStateArray_.getSize() - 1; ++i)
    {
        const Instant& thisStateInstant = this->cachedStateArray_[i].getInstant();
        const Instant& nextStateInstant = this->cachedStateArray_[i + 1].getInstant();

        instants = Array<Instant>::Empty();

        while (j < anInstantArray.getSize())
        {
            if (anInstantArray[j] >= nextStateInstant)
            {
                break;
            }

            if ((anInstantArray[j] >= thisStateInstant) && (anInstantArray[j] < nextStateInstant))
            {
                instants.add(anInstantArray[j]);
            }

            ++j;
        }

        if (instants.isEmpty())
        {
            continue;
        }

        // Forward propagation
        Array<State> forwardStates = propagator_.calculateStatesAt(this->cachedStateArray_[i], instants, maneuvers_);

        // Backward propagation
        Array<State> backwardStates =
            propagator_.calculateStatesAt(this->cachedStateArray_[i + 1], instants, maneuvers_);

        double durationBetweenStates = (nextStateInstant - thisStateInstant).inSeconds();

        // Take weighted average
        Array<State> averagedStates = Array<State>::Empty();
        averagedStates.reserve(instants.getSize());

        for (Size k = 0; k < instants.getSize(); ++k)
        {
            double forwardWeight =
                (this->cachedStateArray_[i + 1].getInstant() - instants[k]).inSeconds() / durationBetweenStates;
            double backwardWeight =
                (instants[k] - this->cachedStateArray_[i].getInstant()).inSeconds() / durationBetweenStates;

            VectorXd coordinates =
                (forwardStates[k].getCoordinates() * forwardWeight + backwardStates[k].getCoordinates() * backwardWeight
                );

            averagedStates.add(outputStateBuilder.build(instants[k], coordinates));
        }

        allStates.add(averagedStates);
    }

    // Propagate any remaining instants forward in time

    instants = Array<Instant>::Empty();

    for (; j < anInstantArray.getSize(); ++j)
    {
        instants.add(anInstantArray[j]);
    }

    allStates.add(propagator_.calculateStatesAt(this->cachedStateArray_.getLast(), instants, maneuvers_));

    return allStates;
}

int Propagated::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (anInstant == this->getEpoch())
    {
        return this->getRevolutionNumberAtEpoch();
    }

    // Find the central body gravity dynamics to use its gravitational parameter for orbital period computation
    std::optional<Derived> gravitationalParameterOptional = std::nullopt;
    for (const Shared<Dynamics>& dynamicsSPtr :
         propagator_.getDynamics())  // Propagator has exactly one Central Body Gravity Dynamics, so
                                     // graviationalParameter will be set after the loop
    {
        if (const auto centralBodyGravitySPtr = std::dynamic_pointer_cast<CentralBodyGravity>(dynamicsSPtr))
        {
            gravitationalParameterOptional = centralBodyGravitySPtr->getCelestial()->getGravitationalParameter();
            break;
        }
    }

    if (!gravitationalParameterOptional.has_value())
    {
        throw ostk::core::error::runtime::Undefined("Central body gravity dynamics");
    }

    const Derived gravitationalParameter = gravitationalParameterOptional.value();

    // Determine whether to count revolution numbers in forwards or backwards time and return function if duration is 0
    const State stateAtEpoch = this->calculateStateAt(this->getEpoch());
    const int durationSign = (anInstant > this->getEpoch()) ? 1 : -1;

    State currentState = stateAtEpoch;
    Instant currentInstant = stateAtEpoch.getInstant();
    int revolutionNumber = this->getRevolutionNumberAtEpoch();

    // Propagate towards desired instant a fraction of an orbit at a time in while loop, exit when arrived at desired
    // instant
    while (true)
    {
        // Calculate orbital period at current state
        const COE coe =
            COE::Cartesian({currentState.getPosition(), currentState.getVelocity()}, gravitationalParameter);
        const Duration orbitalPeriod = coe.getOrbitalPeriod(gravitationalParameter);

        // Propagate for duration of this orbital period
        currentState =
            propagator_.calculateStateAt(currentState, currentInstant + (durationSign * orbitalPeriod), maneuvers_);
        currentInstant = currentState.getInstant();

        // If we have passed the desired instant during our progration, break from the loop
        if ((durationSign >= 0) && currentInstant > anInstant)
            break;
        if ((durationSign <= 0) && currentInstant < anInstant)
            break;

        // Increase or decrease revolution number by 1
        revolutionNumber += durationSign;
    }

    return revolutionNumber;
}

const Array<State>& Propagated::getCachedStateArray() const
{
    return cachedStateArray_;
}

const Propagator& Propagated::getPropagator() const
{
    return propagator_;
}

const Array<Maneuver>& Propagated::getManeuvers() const
{
    return maneuvers_;
}

void Propagated::setCachedStateArray(const Array<State>& aStateArray)
{
    if (aStateArray.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Cached state array is empty.");
    }

    const Array<State> previousCachedStateArray = this->cachedStateArray_;

    this->cachedStateArray_ = aStateArray;

    try
    {
        sanitizeCachedArray();

        this->validateManeuvers();
    }
    catch (...)
    {
        this->cachedStateArray_ = previousCachedStateArray;

        throw;
    }
}

void Propagated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagated") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Cached State Array");
    [&](const Array<State>&) -> void
    {
        for (State iterState : cachedStateArray_)
        {
            iterState.print(anOutputStream, false);
        };
    };

    ostk::core::utils::Print::Separator(anOutputStream, "Propagator");
    propagator_.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Maneuvers");
    ostk::core::utils::Print::Line(anOutputStream) << String::Format("Count: {}", maneuvers_.getSize());

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool Propagated::operator==(const trajectory::Model& aModel) const
{
    const Propagated* propagatedModelPtr = dynamic_cast<const Propagated*>(&aModel);

    return (propagatedModelPtr != nullptr) && this->operator==(*propagatedModelPtr);
}

bool Propagated::operator!=(const trajectory::Model& aModel) const
{
    return !((*this) == aModel);
}

void Propagated::sanitizeCachedArray() const
{
    // Sort cashed state array and remove duplicate states
    std::sort(
        cachedStateArray_.begin(),
        cachedStateArray_.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    cachedStateArray_.erase(std::unique(cachedStateArray_.begin(), cachedStateArray_.end()), cachedStateArray_.end());

    // Check to see if there are any duplicated instants with different positions and velocities
    Array<State> cachedStateArrayUnique(cachedStateArray_);
    cachedStateArrayUnique.erase(
        std::unique(
            cachedStateArrayUnique.begin(),
            cachedStateArrayUnique.end(),
            [](const auto& lhs, const auto& rhs)
            {
                return lhs.getInstant() == rhs.getInstant();
            }
        ),
        cachedStateArrayUnique.end()
    );

    if (cachedStateArray_.getSize() != cachedStateArrayUnique.getSize())
    {
        throw ostk::core::error::runtime::Wrong(
            "State array with States at same instant but different position/velocity were found in cachedStateArray",
            String::Format(
                "Array size: {}, Unique Instants Array size: {}",
                cachedStateArray_.getSize(),
                cachedStateArrayUnique.getSize()
            )
        );
    }
}

void Propagated::validateManeuvers() const
{
    if (maneuvers_.isEmpty())
    {
        return;
    }

    // Overlapping, touching and duplicated maneuvers are all merged into fewer intervals for the check below
    Array<Interval> maneuverIntervals = Array<Interval>::Empty();
    maneuverIntervals.reserve(maneuvers_.getSize());
    for (const Maneuver& maneuver : maneuvers_)
    {
        maneuverIntervals.add(maneuver.getInterval());
    }

    if (Interval::Merge(maneuverIntervals).getSize() != maneuverIntervals.getSize())
    {
        throw ostk::core::error::RuntimeError("Maneuvers must be non-overlapping and unique.");
    }

    for (const State& state : cachedStateArray_)
    {
        if (!state.hasSubset(CoordinateSubset::Mass()))
        {
            throw ostk::core::error::RuntimeError("Cached states must contain the mass when there are maneuvers.");
        }
    }
}

Tabulated Propagated::toTabulated(const Array<Instant>& anInstantArray) const
{
    const Array<State> propagatedStates = this->calculateStatesAt(anInstantArray);

    return Tabulated::Default(
        propagatedStates, DEFAULT_TABULATED_TRAJECTORY_OUTPUT_FRAME, this->initialRevolutionNumber_
    );
}

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
