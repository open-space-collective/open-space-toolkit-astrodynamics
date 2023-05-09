// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

using ostk::core::types::Size;

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);
static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

Propagated::Propagated(
    const SatelliteDynamics& aSatelliteDynamics, const NumericalSolver& aNumericalSolver, const State& aState
)
    : Model(),
      propagator_(aSatelliteDynamics, aNumericalSolver),
      cachedStateArray_(1, aState)

{
}

Propagated::Propagated(
    const SatelliteDynamics& aSatelliteDynamics,
    const NumericalSolver& aNumericalSolver,
    const Array<State>& aCachedStateArray
)
    : Model(),
      propagator_(aSatelliteDynamics, aNumericalSolver),
      cachedStateArray_(aCachedStateArray)

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

    return 1;  // [TBI] With param
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
        throw ostk::core::error::runtime::Wrong("Propagated state array size");
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
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array");
        }
    }

    Array<State> allStates = Array<State>::Empty();

    // Maintain counter separately so as to only iterate once through instant array

    Size j = 0;

    // Propagate states for instants < first cached State

    Array<Instant> instants = Array<Instant>::Empty();
    for (; j < anInstantArray.getSize(); ++j)
    {
        if (anInstantArray[j] > this->cachedStateArray_.accessFirst().getInstant())
        {
            break;
        }

        instants.add(anInstantArray[j]);
    }

    allStates.add(propagator_.calculateStatesAt(this->cachedStateArray_.accessFirst(), instants));

    // Propagate all instants between states

    for (Size i = 0; i < this->cachedStateArray_.getSize() - 1; ++i)
    {
        const Instant thisStateInstant = this->cachedStateArray_[i].getInstant();
        const Instant nextStateInstant = this->cachedStateArray_[i + 1].getInstant();

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
                (this->cachedStateArray_[i + 1].getInstant() - instants[k]).inSeconds() / durationBetweenStates;
            Real backwardWeight =
                (instants[k] - this->cachedStateArray_[i].getInstant()).inSeconds() / durationBetweenStates;

            VectorXd coordinates =
                (forwardStates[k].getCoordinates() * forwardWeight + backwardStates[k].getCoordinates() * backwardWeight
                );

            averagedStates.add(
                {instants[k],
                 Position::Meters({coordinates[0], coordinates[1], coordinates[2]}, gcrfSPtr),
                 Velocity::MetersPerSecond({coordinates[3], coordinates[4], coordinates[5]}, gcrfSPtr)}
            );
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

    if (anInstant == cachedStateArray_[0].getInstant())
    {
        return this->getRevolutionNumberAtEpoch();
    }

    // Calculate gravitational parameter (Spherical earth has the most modern value which is the correct one)
    using ostk::physics::env::obj::celest::Earth;

    const Derived gravitationalParameter = Earth::Models::Spherical::GravitationalParameter;
    const Real gravitationalParameter_SI = gravitationalParameter.in(GravitationalParameterSIUnit);

    Position currentPosition = cachedStateArray_[0].getPosition();
    Velocity currentVelocity = cachedStateArray_[0].getVelocity();

    Vector3d currentPositionCoordinates = currentPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    Vector3d currentVelocityCoordinates = currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    // Determine whether to count revolution numbers in forwards or backwards time and return function if duration is 0
    Instant currentInstant = cachedStateArray_[0].getInstant();
    const double durationInSecs = (anInstant - currentInstant).inSeconds();
    if (durationInSecs == 0.0)
    {
        return 1;
    }
    Integer durationSign = (durationInSecs > 0) - (durationInSecs < 0);

    // Propagate towards desired instant a fraction of an orbit at a time in while loop, exit when arrived at desired
    // instant
    Integer revolutionNumber = 0;
    while (true)
    {
        // Calculate orbital period
        const double semiMajorAxis =
            -gravitationalParameter_SI * currentPositionCoordinates.norm() /
            (currentPositionCoordinates.norm() * std::pow(currentVelocityCoordinates.norm(), 2) -
             2.0 * gravitationalParameter_SI);
        const Duration orbitalPeriod =
            Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis, 3) / gravitationalParameter_SI));

        // If we have passed the desired instant during our progration, break from the loop
        if (durationSign.isPositive() && currentInstant > anInstant)
            break;
        if (durationSign.isNegative() && currentInstant < anInstant)
            break;

        // Increase or decrease revolution number by 1
        revolutionNumber += durationSign;

        // Propagate for duration of this orbital period
        const State currentState = propagator_.calculateStateAt(
            cachedStateArray_[0], cachedStateArray_[0].getInstant() + (durationSign * orbitalPeriod)
        );

        // Update the current instant position and velocity coordinates
        currentPositionCoordinates = currentState.getPosition().getCoordinates();
        currentVelocityCoordinates = currentState.getVelocity().getCoordinates();
        currentInstant += durationSign * orbitalPeriod;
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
            "State array with States at same instant but different position/velocity were found in cachedStateArray"
        );
    }
}

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
