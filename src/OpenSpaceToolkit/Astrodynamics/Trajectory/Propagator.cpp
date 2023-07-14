/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
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

using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::AtmosphericDrag;

static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamics_(aDynamicsArray),
      numericalSolver_(aNumericalSolver)
{
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

Array<Shared<Dynamics>> Propagator::getDynamics() const
{
    return this->dynamics_;
}

void Propagator::setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray)
{
    this->dynamics_ = aDynamicsArray;
}

void Propagator::addDynamics(const Shared<Dynamics>& aDynamics)
{
    if (aDynamics == nullptr || !aDynamics->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    this->dynamics_.add(aDynamics);
}

void Propagator::clearDynamics()
{
    this->dynamics_.clear();
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const NumericalSolver::StateVector startStateVector = aState.getCoordinates();

    const NumericalSolver::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(
        startStateVector,
        aState.getInstant(),
        anInstant,
        Dynamics::GetDynamicalEquations(this->dynamics_, aState.getInstant())
    );

    return {
        anInstant,
        Position::Meters({endStateVector[0], endStateVector[1], endStateVector[2]}, gcrfSPtr),
        Velocity::MetersPerSecond({endStateVector[3], endStateVector[4], endStateVector[5]}, gcrfSPtr),
    };
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

    const NumericalSolver::StateVector startStateVector = aState.getCoordinates();

    Array<Instant> forwardInstants;
    forwardInstants.reserve(anInstantArray.getSize());
    Array<Instant> backwardInstants;
    backwardInstants.reserve(anInstantArray.getSize());

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
    Array<NumericalSolver::StateVector> propagatedForwardStateVectorArray;
    if (!forwardInstants.isEmpty())
    {
        propagatedForwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector,
            aState.getInstant(),
            forwardInstants,
            Dynamics::GetDynamicalEquations(this->dynamics_, aState.getInstant())
        );
    }

    // backward propagation only
    Array<NumericalSolver::StateVector> propagatedBackwardStateVectorArray;
    if (!backwardInstants.isEmpty())
    {
        std::reverse(backwardInstants.begin(), backwardInstants.end());

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector,
            aState.getInstant(),
            backwardInstants,
            Dynamics::GetDynamicalEquations(this->dynamics_, aState.getInstant())
        );

        std::reverse(propagatedBackwardStateVectorArray.begin(), propagatedBackwardStateVectorArray.end());
    }

    Array<State> propagatedStates;
    propagatedStates.reserve(anInstantArray.getSize());

    Size k = 0;
    for (const NumericalSolver::StateVector& stateVector :
         (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {
        State propagatedState = {
            anInstantArray[k],
            Position::Meters({stateVector[0], stateVector[1], stateVector[2]}, gcrfSPtr),
            Velocity::MetersPerSecond({stateVector[3], stateVector[4], stateVector[5]}, gcrfSPtr),
        };

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
    return {
        NumericalSolver::Undefined(),
        Array<Shared<Dynamics>>::Empty(),
    };
}

Propagator Propagator::Default()
{
    return {
        NumericalSolver::Default(),
        {std::make_shared<PositionDerivative>()},
    };
}

Propagator Propagator::Default(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem)
{
    return Propagator::FromEnvironment(NumericalSolver::Default(), anEnvironment, aSatelliteSystem);
}

Propagator Propagator::FromEnvironment(
    const NumericalSolver& aNumericalSolver, const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem
)
{
    const auto getDynamics = [aSatelliteSystem](const Shared<const Celestial>& aCelestial) -> Array<Shared<Dynamics>>
    {
        Array<Shared<Dynamics>> dynamics = Array<Shared<Dynamics>>::Empty();

        if (aCelestial->gravitationalModelIsDefined())
        {
            if (aCelestial->getName() == "Earth")
            {
                dynamics.add(std::make_shared<CentralBodyGravity>(aCelestial));
            }
            else
            {
                dynamics.add(std::make_shared<ThirdBodyGravity>(aCelestial));
            }
        }

        if (aCelestial->atmosphericModelIsDefined())
        {
            dynamics.add(std::make_shared<AtmosphericDrag>(aCelestial, aSatelliteSystem));
        }

        return dynamics;
    };

    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const String& name : anEnvironment.getObjectNames())
    {
        const Shared<const Celestial> celestialSPtr = anEnvironment.accessCelestialObjectWithName(name);

        dynamicsArray.add(getDynamics(celestialSPtr));
    }

    dynamicsArray.add(std::make_shared<PositionDerivative>());

    return {
        aNumericalSolver,
        dynamicsArray,
    };
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
