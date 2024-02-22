/// Apache License 2.0

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
const Map<String, Pair<Size, Size>> Propagator::ValidDynamicsSet = {
    {"CentralBodyGravity", {1, 1}},                        // Minimum of 1 and maximum of 1
    {"PositionDerivative", {1, 1}},                        // Minimum of 1 and maximum of 1
    {"ThirdBodyGravity", {0, 2}},                          // Minimum of 0 and maximum of 2
    {"AtmosphericDrag", {0, 1}},                           // Minimum of 0 and maximum of 1
    {"Thruster", {0, 1}},                                  // Minimum of 0 and maximum of 1
    {"Tabulated", {0, std::numeric_limits<Size>::max()}},  // Minimum of 0 and maximum of the maximum size of Size
};

Propagator::Propagator(const NumericalSolver& aNumericalSolver, const Array<Shared<Dynamics>>& aDynamicsArray)
    : dynamicsContexts_(),
      numericalSolver_(aNumericalSolver)
{
    this->setDynamics(aDynamicsArray);
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
    Map<String, Size> currentDynamicsSets = {
        {"CentralBodyGravity", 0},
        {"PositionDerivative", 0},
        {"ThirdBodyGravity", 0},
        {"AtmosphericDrag", 0},
        {"Thruster", 0},
        {"Tabulated", 0},
    };

    for (const Dynamics::Context& dynamicsContext : dynamicsContexts_)
    {
        if (std::dynamic_pointer_cast<CentralBodyGravity>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["CentralBodyGravity"]++;
        }
        else if (std::dynamic_pointer_cast<PositionDerivative>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["PositionDerivative"]++;
        }
        else if (std::dynamic_pointer_cast<ThirdBodyGravity>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["ThirdBodyGravity"]++;
        }
        else if (std::dynamic_pointer_cast<AtmosphericDrag>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["AtmosphericDrag"]++;
        }
        else if (std::dynamic_pointer_cast<Thruster>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["Thruster"]++;
        }
        else if (std::dynamic_pointer_cast<TabulatedDynamics>(dynamicsContext.dynamics))
        {
            currentDynamicsSets["Tabulated"]++;
        }
        else
        {
            throw ostk::core::error::RuntimeError("Unrecognied Dynamics was inputted into this propagator.");
        }
    }

    for (const auto& currentDynamics : currentDynamicsSets)
    {
        if (currentDynamics.second < Propagator::ValidDynamicsSet.at(currentDynamics.first).first ||
            currentDynamics.second > Propagator::ValidDynamicsSet.at(currentDynamics.first).second)
        {
            throw ostk::core::error::RuntimeError("Invalid Dynamics Set.");
        }
    }
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
