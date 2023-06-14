/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/GravitationalDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Size;
using ostk::core::types::Index;

using ostk::math::obj::VectorXd;

using ostk::physics::env::obj::Celestial;

using ostk::astro::flight::system::dynamics::GravitationalDynamics;
using ostk::astro::flight::system::dynamics::AtmosphericDynamics;

static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamics_(aDynamicsArray),
      numericalSolver_(aNumericalSolver)
{
}

Propagator::Propagator(
    const NumericalSolver& aNumericalSolver, const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem
)
    : dynamics_(Array<Shared<Dynamics>>::Empty()),
      numericalSolver_(aNumericalSolver)
{
    for (const String& name : anEnvironment.getObjectNames())
    {
        // TBI: We should come up with a way to avoid the cloning here, somewhat defeats the purpose.
        const Shared<Celestial> celestial(anEnvironment.accessCelestialObjectWithName(name)->clone());
        if (celestial->accessGravitationalModel()->isDefined())
        {
            const Shared<Dynamics> dynamics = std::make_shared<GravitationalDynamics>(GravitationalDynamics(celestial));
            this->addDynamics(dynamics);
        }
        else if (celestial->accessAtmosphericModel()->isDefined())
        {
            const Shared<Dynamics> dynamics =
                std::make_shared<AtmosphericDynamics>(AtmosphericDynamics(celestial, aSatelliteSystem));
            this->addDynamics(dynamics);
        }
    }
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (numericalSolver_ == aPropagator.numericalSolver_ && dynamics_ == aPropagator.dynamics_);
}

bool Propagator::operator!=(const Propagator& aPropagator) const
{
    return !((*this) == aPropagator);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Propagator& aPropagator)
{
    aPropagator.print(anOutputStream);

    return anOutputStream;
}

bool Propagator::isDefined() const
{
    return numericalSolver_.isDefined();
}

void Propagator::setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray)
{
    this->dynamics_ = aDynamicsArray;
}

void Propagator::addDynamics(const Shared<Dynamics>& aDynamics)
{
    this->dynamics_.add(aDynamics);
}

void Propagator::clearDynamics()
{
    this->dynamics_.clear();
}

Array<Shared<Dynamics>> Propagator::getDynamics() const
{
    return this->dynamics_;
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const VectorXd stateCoordinates = aState.getCoordinates();

    Dynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size());

    const Dynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(
        startStateVector,
        aState.getInstant(),
        anInstant,
        Dynamics::GetDynamicalEquations(aState.getInstant(), this->dynamics_)
    );

    return {
        anInstant,
        Position::Meters({endStateVector[0], endStateVector[1], endStateVector[2]}, gcrfSPtr),
        Velocity::MetersPerSecond({endStateVector[3], endStateVector[4], endStateVector[5]}, gcrfSPtr)};
}

Array<State> Propagator::calculateStatesAt(const State& aState, const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
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

    const VectorXd stateCoordinates = aState.getCoordinates();
    Dynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size());

    Array<Instant> forwardInstants;
    Array<Instant> backwardInstants;

    for (const Instant& anInstant : anInstantArray)
    {
        if (anInstant <= aState.getInstant())
        {
            backwardInstants.add(anInstant);
        }
        else
        {
            forwardInstants.add(anInstant);
        }
    }

    // forward propagation only
    Array<Dynamics::StateVector> propagatedForwardStateVectorArray;
    if (!forwardInstants.isEmpty())
    {
        propagatedForwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector,
            aState.getInstant(),
            forwardInstants,
            Dynamics::GetDynamicalEquations(aState.getInstant(), this->dynamics_)
        );
    }

    // backward propagation only
    Array<Dynamics::StateVector> propagatedBackwardStateVectorArray;
    if (!backwardInstants.isEmpty())
    {
        std::reverse(backwardInstants.begin(), backwardInstants.end());

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector,
            aState.getInstant(),
            backwardInstants,
            Dynamics::GetDynamicalEquations(aState.getInstant(), this->dynamics_)
        );

        std::reverse(propagatedBackwardStateVectorArray.begin(), propagatedBackwardStateVectorArray.end());
    }

    Array<State> propagatedStates;
    propagatedStates.reserve(anInstantArray.getSize());

    Size k = 0;
    for (const Dynamics::StateVector& stateVector :
         (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {
        State propagatedState = {
            anInstantArray[k],
            Position::Meters({stateVector[0], stateVector[1], stateVector[2]}, gcrfSPtr),
            Velocity::MetersPerSecond({stateVector[3], stateVector[4], stateVector[5]}, gcrfSPtr)};

        propagatedStates.add(propagatedState);

        ++k;
    }

    return propagatedStates;
}

void Propagator::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagator") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver");
    numericalSolver_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Propagator Propagator::Undefined()
{
    return {NumericalSolver::Undefined(), Array<Shared<Dynamics>>::Empty()};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
