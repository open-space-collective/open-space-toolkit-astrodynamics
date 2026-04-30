/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
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
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::StateBuilder;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

Propagated::Propagated(const Propagator& aPropagator, const State& aState, const Integer& aRevolutionNumber)
    : Model(),
      propagator_(aPropagator),
      cachedStateArray_(1, aState),
      initialRevolutionNumber_(aRevolutionNumber)
{
}

Propagated::Propagated(
    const Propagator& aPropagator, const Array<State>& aCachedStateArray, const Integer& aRevolutionNumber
)
    : Model(),
      propagator_(aPropagator),
      cachedStateArray_(aCachedStateArray),
      initialRevolutionNumber_(aRevolutionNumber)
{
    sanitizeCachedArray();
}

Propagated* Propagated::clone() const
{
    return new Propagated(*this);
}

bool Propagated::operator==(const Propagated& aPropagatedModel) const
{
    if ((!this->isDefined()) || (!aPropagatedModel.isDefined()))
    {
        return false;
    }

    return (cachedStateArray_ == aPropagatedModel.cachedStateArray_) && (propagator_ == aPropagatedModel.propagator_);
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

bool Propagated::isDefined() const
{
    return !cachedStateArray_.isEmpty() && propagator_.isDefined();
}

Instant Propagated::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    return cachedStateArray_[0].getInstant();
}

Integer Propagated::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    return this->initialRevolutionNumber_;
}

State Propagated::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

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
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

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
    StateBuilder outputStateBuilder = {this->cachedStateArray_.accessFirst()};

    Array<State> allStates = Array<State>::Empty();

    // Maintain counter separately so as to only iterate once through instant array

    Size j = 0;

    // Propagate states for instants < first cached State

    Array<Instant> instants = Array<Instant>::Empty();
    for (; j < anInstantArray.getSize(); ++j)
    {
        if (anInstantArray[j] > this->cachedStateArray_.accessFirst().accessInstant())
        {
            break;
        }

        instants.add(anInstantArray[j]);
    }

    allStates.add(propagator_.calculateStatesAt(this->cachedStateArray_.accessFirst(), instants));

    // Propagate all instants between states

    for (Size i = 0; i < this->cachedStateArray_.getSize() - 1; ++i)
    {
        const Instant& thisStateInstant = this->cachedStateArray_[i].accessInstant();
        const Instant& nextStateInstant = this->cachedStateArray_[i + 1].accessInstant();

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
        Array<State> forwardStates = propagator_.calculateStatesAt(this->cachedStateArray_[i], instants);

        // Backward propagation
        Array<State> backwardStates = propagator_.calculateStatesAt(this->cachedStateArray_[i + 1], instants);

        Real durationBetweenStates = (nextStateInstant - thisStateInstant).inSeconds();

        // Take weighted average
        Array<State> averagedStates = Array<State>::Empty();
        averagedStates.reserve(instants.getSize());

        for (Size k = 0; k < instants.getSize(); ++k)
        {
            Real forwardWeight =
                (this->cachedStateArray_[i + 1].accessInstant() - instants[k]).inSeconds() / durationBetweenStates;
            Real backwardWeight =
                (instants[k] - this->cachedStateArray_[i].accessInstant()).inSeconds() / durationBetweenStates;

            VectorXd coordinates =
                (forwardStates[k].accessCoordinates() * forwardWeight +
                 backwardStates[k].accessCoordinates() * backwardWeight);

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

    allStates.add(propagator_.calculateStatesAt(this->cachedStateArray_.accessLast(), instants));

    return allStates;
}

Integer Propagated::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    if (anInstant == this->getEpoch())
    {
        return this->getRevolutionNumberAtEpoch();
    }

    // Determine whether to count revolution numbers in forwards or backwards time and return function if duration is 0
    const State stateAtEpoch = this->calculateStateAt(this->getEpoch());
    const Integer durationSign = (anInstant > this->getEpoch()) ? 1 : -1;

    State currentState = stateAtEpoch;
    Instant currentInstant = stateAtEpoch.accessInstant();
    Integer revolutionNumber = this->getRevolutionNumberAtEpoch();

    // Propagate towards desired instant a fraction of an orbit at a time in while loop, exit when arrived at desired
    // instant
    while (true)
    {
        // Calculate orbital period at current state
        const COE coe = COE::Cartesian(
            {currentState.getPosition(), currentState.getVelocity()}, Earth::Spherical.gravitationalParameter_
        );  // (Spherical earth has the most modern value which is the correct one)
        const Duration orbitalPeriod = coe.getOrbitalPeriod(Earth::Spherical.gravitationalParameter_);

        // Propagate for duration of this orbital period
        currentState = propagator_.calculateStateAt(currentState, currentInstant + (durationSign * orbitalPeriod));
        currentInstant = currentState.accessInstant();

        // If we have passed the desired instant during our progration, break from the loop
        if (durationSign.isPositive() && currentInstant > anInstant)
            break;
        if (durationSign.isNegative() && currentInstant < anInstant)
            break;

        // Increase or decrease revolution number by 1
        revolutionNumber += durationSign;
    }

    return revolutionNumber;
}

const Array<State>& Propagated::accessCachedStateArray() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    return cachedStateArray_;
}

const Propagator& Propagated::accessPropagator() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    return propagator_;
}

void Propagated::setCachedStateArray(const Array<State>& aStateArray)
{
    this->cachedStateArray_ = aStateArray;

    sanitizeCachedArray();
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
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated");
    }

    // Sort cashed state array and remove duplicate states
    std::sort(
        cachedStateArray_.begin(),
        cachedStateArray_.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.accessInstant() < rhs.accessInstant();
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
                return lhs.accessInstant() == rhs.accessInstant();
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

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
