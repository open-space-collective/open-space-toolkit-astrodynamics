/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::type::Index;
using ostk::core::container::Pair;

using ostk::mathematics::object::VectorXd;

using ostk::physics::environment::object::Celestial;

using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::ThirdBodyGravity;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::trajectory::state::CoordinatesSubset;

const Shared<const Frame> Propagator::IntegrationFrameSPtr = Frame::GCRF();

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamicsContexts_(),
      numericalSolver_(aNumericalSolver)
{
    for (const Shared<Dynamics>& aDynamicsSPtr : aDynamicsArray)
    {
        this->registerDynamicsContext(aDynamicsSPtr);
    }
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (
        numericalSolver_ == aPropagator.numericalSolver_ &&
        *this->coordinatesBrokerSPtr_ == *(aPropagator.coordinatesBrokerSPtr_)
    );
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
    return this->numericalSolver_.isDefined() && this->coordinatesBrokerSPtr_ != nullptr &&
           !this->dynamicsContexts_.isEmpty();
}

const Shared<CoordinatesBroker>& Propagator::accessCoordinatesBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return this->coordinatesBrokerSPtr_;
}

const NumericalSolver& Propagator::accessNumericalSolver() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return this->numericalSolver_;
}

Size Propagator::getNumberOfCoordinates() const
{
    return this->accessCoordinatesBroker()->getNumberOfCoordinates();
}

Array<Shared<Dynamics>> Propagator::getDynamics() const
{
    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const Dynamics::Context& dynamicsContext : this->dynamicsContexts_)
    {
        dynamicsArray.add(dynamicsContext.dynamics);
    }

    return dynamicsArray;
}

void Propagator::setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray)
{
    this->clearDynamics();

    for (const Shared<Dynamics>& aDynamicsSPtr : aDynamicsArray)
    {
        this->registerDynamicsContext(aDynamicsSPtr);
    }
}

void Propagator::addDynamics(const Shared<Dynamics>& aDynamicsSPtr)
{
    if (aDynamicsSPtr == nullptr || !aDynamicsSPtr->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    this->registerDynamicsContext(aDynamicsSPtr);
}

void Propagator::clearDynamics()
{
    this->dynamicsContexts_.clear();
    this->coordinatesBrokerSPtr_ = std::make_shared<CoordinatesBroker>();
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const StateBuilder solverStateBuilder = {Propagator::IntegrationFrameSPtr, coordinatesBrokerSPtr_};

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    const State solverOutputState = numericalSolver_.integrateTime(
        solverInputState,
        anInstant,
        Dynamics::GetSystemOfEquations(
            this->dynamicsContexts_, solverInputState.accessInstant(), Propagator::IntegrationFrameSPtr
        )
    );

    const StateBuilder outputStateBuilder = {aState};

    return outputStateBuilder.expand(solverOutputState.inFrame(aState.accessFrame()), aState);
}

NumericalSolver::ConditionSolution Propagator::calculateStateToCondition(
    const State& aState, const Instant& anInstant, const EventCondition& anEventCondition
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    const Instant& startInstant = aState.accessInstant();

    const StateBuilder solverStateBuilder = {Propagator::IntegrationFrameSPtr, coordinatesBrokerSPtr_};

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    NumericalSolver::ConditionSolution conditionSolution = numericalSolver_.integrateTime(
        solverInputState,
        anInstant,
        Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr),
        anEventCondition
    );

    const StateBuilder outputStateBuilder = {aState};

    conditionSolution.state = outputStateBuilder.expand(conditionSolution.state.inFrame(aState.accessFrame()), aState);

    return conditionSolution;
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

    const StateBuilder solverStateBuilder = {Propagator::IntegrationFrameSPtr, coordinatesBrokerSPtr_};

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    const Instant& startInstant = solverInputState.accessInstant();

    Array<Instant> forwardInstants;
    forwardInstants.reserve(anInstantArray.getSize());
    Array<Instant> backwardInstants;
    backwardInstants.reserve(anInstantArray.getSize());

    const StateBuilder outputStateBuilder(aState);

    for (const Instant& anInstant : anInstantArray)
    {
        if (anInstant <= startInstant)
        {
            backwardInstants.add(anInstant);
        }
        else
        {
            forwardInstants.add(anInstant);
        }
    }

    // forward propagation only
    Array<State> forwardPropagatedStates;
    if (!forwardInstants.isEmpty())
    {
        forwardPropagatedStates = numericalSolver_.integrateTime(
            solverInputState,
            forwardInstants,
            Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr)
        );
    }

    // backward propagation only
    Array<State> backwardPropagatedStates;
    if (!backwardInstants.isEmpty())
    {
        std::reverse(backwardInstants.begin(), backwardInstants.end());

        backwardPropagatedStates = numericalSolver_.integrateTime(
            solverInputState,
            backwardInstants,
            Dynamics::GetSystemOfEquations(this->dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr)
        );

        std::reverse(backwardPropagatedStates.begin(), backwardPropagatedStates.end());
    }

    Array<State> outputStates;
    outputStates.reserve(backwardPropagatedStates.getSize() + forwardPropagatedStates.getSize());

    for (const State& solverOutputState : backwardPropagatedStates + forwardPropagatedStates)
    {
        outputStates.add(outputStateBuilder.expand(solverOutputState.inFrame(aState.accessFrame()), aState));
    }

    return outputStates;
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

Propagator Propagator::Default(const Environment& anEnvironment)
{
    return Propagator::FromEnvironment(NumericalSolver::Default(), anEnvironment);
}

Propagator Propagator::FromEnvironment(const NumericalSolver& aNumericalSolver, const Environment& anEnvironment)
{
    return {
        aNumericalSolver,
        Dynamics::FromEnvironment(anEnvironment),
    };
}

void Propagator::registerDynamicsContext(const Shared<Dynamics>& aDynamicsSPtr)
{
    // Store read coordinate subsets information
    Array<Pair<Index, Size>> readInfo = Array<Pair<Index, Size>>::Empty();
    readInfo.reserve(aDynamicsSPtr->getReadCoordinatesSubsets().getSize());
    for (const Shared<const CoordinatesSubset>& subset : aDynamicsSPtr->getReadCoordinatesSubsets())
    {
        const Pair<Index, Size> indexAndSize = {this->coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        readInfo.add(indexAndSize);
    }

    // Store write coordinate subsets information
    Array<Pair<Index, Size>> writeInfo = Array<Pair<Index, Size>>::Empty();
    writeInfo.reserve(aDynamicsSPtr->getWriteCoordinatesSubsets().getSize());
    for (const Shared<const CoordinatesSubset>& subset : aDynamicsSPtr->getWriteCoordinatesSubsets())
    {
        const Pair<Index, Size> indexAndSize = {this->coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        writeInfo.add(indexAndSize);
    }

    this->dynamicsContexts_.add({aDynamicsSPtr, readInfo, writeInfo});
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
