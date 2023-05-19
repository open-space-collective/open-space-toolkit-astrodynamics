/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Size;

using ostk::physics::env::obj::celest::Earth;

static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

Propagator::Propagator(const Array<Shared<Dynamics>>& aDynamicsArray, const NumericalSolver& aNumericalSolver)
    : dynamics_(aDynamicsArray),
      numericalSolver_(aNumericalSolver)

{
}

Propagator* Propagator::clone() const
{
    return new Propagator(*this);
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (numericalSolver_ == aPropagator.numericalSolver_);
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

Propagator::DynamicalEquationWrapper Propagator::getDynamicalEquations(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return std::bind(
        Propagator::DynamicalEquations,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        this->dynamics_,
        anInstant
    );
}

void Propagator::DynamicalEquations(
    const Dynamics::StateVector& x,
    Dynamics::StateVector& dxdt,
    const double t,
    const Array<Shared<Dynamics>>& dynamics,
    const Instant& anInstant
)
{
    const Real radius = std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);

    // Check for radii below 70km altitude
    if (radius < Earth::Models::EGM2008::EquatorialRadius.inMeters() + 70000.0)
    {
        throw ostk::core::error::RuntimeError("Satellite altitude too low, has re-entered.");
    }

    for (const Shared<Dynamics>& dynamic : dynamics)
    {
        dynamic->update(x, dxdt, anInstant + Duration::Seconds(t));
    }
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const VectorXd stateCoordinates = aState.getCoordinates();

    const Dynamics::StateVector startStateVector(
        stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()
    );

    const Dynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(
        startStateVector, aState.getInstant(), anInstant, this->getDynamicalEquations(anInstant)
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
            startStateVector, aState.getInstant(), forwardInstants, this->getDynamicalEquations(aState.getInstant())
        );
    }

    // backward propagation only
    Array<Dynamics::StateVector> propagatedBackwardStateVectorArray;
    if (!backwardInstants.isEmpty())
    {
        std::reverse(backwardInstants.begin(), backwardInstants.end());

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(
            startStateVector, aState.getInstant(), backwardInstants, this->getDynamicalEquations(aState.getInstant())
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

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
