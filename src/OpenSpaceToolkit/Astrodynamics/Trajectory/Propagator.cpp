/// Apache License 2.0

#include <typeindex>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::mathematics::object::VectorXd;

using ostk::physics::environment::object::Celestial;

using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

const Shared<const Frame> Propagator::IntegrationFrameSPtr = Frame::GCRF();

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamicsContexts_(),
      numericalSolver_(aNumericalSolver)
{
    this->setDynamics(aDynamicsArray);
}

Propagator::Propagator(
    const NumericalSolver& aNumericalSolver,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Array<Maneuver>& aManeuverArray,
    const Interpolator::Type& anInterpolationType
)
    : Propagator(aNumericalSolver, aDynamicsArray)
{
    // TBM: in the future maybe sanitize the instants of each maneuver to make sure none of them are overalapping
    for (const Maneuver& maneuver : aManeuverArray)
    {
        this->addManeuver(maneuver, anInterpolationType);
    }
}

Propagator::Propagator(const Propagator& aPropagator)
    : coordinatesBrokerSPtr_(std::make_shared<CoordinateBroker>(*aPropagator.coordinatesBrokerSPtr_)),
      dynamicsContexts_(aPropagator.dynamicsContexts_),
      numericalSolver_(aPropagator.numericalSolver_)
{
}

Propagator& Propagator::operator=(const Propagator& aPropagator)
{
    if (this != &aPropagator)
    {
        coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>(*aPropagator.coordinatesBrokerSPtr_);
        dynamicsContexts_ = aPropagator.dynamicsContexts_;
        numericalSolver_ = aPropagator.numericalSolver_;
    }
    return *this;
}

bool Propagator::operator==(const Propagator& aPropagator) const
{
    if ((!this->isDefined()) || (!aPropagator.isDefined()))
    {
        return false;
    }

    return (
        numericalSolver_ == aPropagator.numericalSolver_ &&
        *(coordinatesBrokerSPtr_) == *(aPropagator.coordinatesBrokerSPtr_)
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
    return numericalSolver_.isDefined() && coordinatesBrokerSPtr_ != nullptr && !dynamicsContexts_.isEmpty();
}

const Shared<CoordinateBroker>& Propagator::accessCoordinateBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return coordinatesBrokerSPtr_;
}

const NumericalSolver& Propagator::accessNumericalSolver() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    return numericalSolver_;
}

Size Propagator::getNumberOfCoordinates() const
{
    return this->accessCoordinateBroker()->getNumberOfCoordinates();
}

Array<Shared<Dynamics>> Propagator::getDynamics() const
{
    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const Dynamics::Context& dynamicsContext : dynamicsContexts_)
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
        this->addDynamics(aDynamicsSPtr);
    }
}

void Propagator::addManeuver(const Maneuver& aManeuver, const Interpolator::Type& anInterpolationType)
{
    this->addDynamics(aManeuver.toTabulatedDynamics(IntegrationFrameSPtr, anInterpolationType));
}

void Propagator::addDynamics(const Shared<Dynamics>& aDynamicsSPtr)
{
    if (aDynamicsSPtr == nullptr || !aDynamicsSPtr->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    // Store read coordinate subsets information
    Array<Pair<Index, Size>> readInfo = Array<Pair<Index, Size>>::Empty();
    readInfo.reserve(aDynamicsSPtr->getReadCoordinateSubsets().getSize());
    for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getReadCoordinateSubsets())
    {
        const Pair<Index, Size> indexAndSize = {coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        readInfo.add(indexAndSize);
    }

    // Store write coordinate subsets information
    Array<Pair<Index, Size>> writeInfo = Array<Pair<Index, Size>>::Empty();
    writeInfo.reserve(aDynamicsSPtr->getWriteCoordinateSubsets().getSize());
    for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getWriteCoordinateSubsets())
    {
        const Pair<Index, Size> indexAndSize = {coordinatesBrokerSPtr_->addSubset(subset), subset->getSize()};
        writeInfo.add(indexAndSize);
    }

    dynamicsContexts_.add({aDynamicsSPtr, readInfo, writeInfo});
}

void Propagator::clearDynamics()
{
    dynamicsContexts_.clear();
    coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>();
}

State Propagator::calculateStateAt(const State& aState, const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    this->validateDynamicsSet();

    const StateBuilder solverStateBuilder = {Propagator::IntegrationFrameSPtr, coordinatesBrokerSPtr_};

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    const State solverOutputState = numericalSolver_.integrateTime(
        solverInputState,
        anInstant,
        Dynamics::GetSystemOfEquations(
            dynamicsContexts_, solverInputState.accessInstant(), Propagator::IntegrationFrameSPtr
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

    this->validateDynamicsSet();

    const Instant& startInstant = aState.accessInstant();

    const StateBuilder solverStateBuilder = {Propagator::IntegrationFrameSPtr, coordinatesBrokerSPtr_};

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    NumericalSolver::ConditionSolution conditionSolution = numericalSolver_.integrateTime(
        solverInputState,
        anInstant,
        Dynamics::GetSystemOfEquations(dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr),
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

    this->validateDynamicsSet();

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
            Dynamics::GetSystemOfEquations(dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr)
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
            Dynamics::GetSystemOfEquations(dynamicsContexts_, startInstant, Propagator::IntegrationFrameSPtr)
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

void Propagator::validateDynamicsSet() const
{
    Map<std::type_index, Array<Shared<Dynamics>>> dynamicsMap;

    for (const Dynamics::Context& dynamicsContext : dynamicsContexts_)
    {
        (dynamicsMap[typeid(*dynamicsContext.dynamics)]).add(dynamicsContext.dynamics);
    }

    // Check for mandatory dynamics
    const auto centralBodyGravityIt = dynamicsMap.find(std::type_index(typeid(CentralBodyGravity)));
    if (centralBodyGravityIt == dynamicsMap.end() || centralBodyGravityIt->second.getSize() != 1)
    {
        throw ostk::core::error::RuntimeError("Propagator needs exactly one Central Body Gravity Dynamics.");
    }

    const auto positionDerivativeIt = dynamicsMap.find(std::type_index(typeid(PositionDerivative)));
    if (positionDerivativeIt == dynamicsMap.end() || positionDerivativeIt->second.getSize() != 1)
    {
        throw ostk::core::error::RuntimeError("Propagator needs exactly one Position Derivative Dynamics.");
    }

    // Check for optional dynamics number limits
    const auto atmosphericDragIt = dynamicsMap.find(std::type_index(typeid(AtmosphericDrag)));
    if (atmosphericDragIt != dynamicsMap.end() && atmosphericDragIt->second.getSize() > 1)
    {
        throw ostk::core::error::RuntimeError("Propagator can have at most one Atmospheric Drag Dynamics.");
    }

    const auto thrusterIt = dynamicsMap.find(std::type_index(typeid(Thruster)));
    if (thrusterIt != dynamicsMap.end() && thrusterIt->second.getSize() > 1)
    {
        throw ostk::core::error::RuntimeError("Propagator can have at most one Thruster Dynamics.");
    }
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
