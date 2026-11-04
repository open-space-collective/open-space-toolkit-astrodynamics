/// Apache License 2.0

#include <algorithm>
#include <typeindex>

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::type::Index;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Position;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Interval;

using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

const Shared<const Frame> Propagator::IntegrationFrameSPtr = Frame::GCRF();

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

    dynamicsContexts_.add(Propagator::CreateContext(aDynamicsSPtr, coordinatesBrokerSPtr_));
}

void Propagator::clearDynamics()
{
    dynamicsContexts_.clear();
    coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>();
}

State Propagator::calculateStateAt(
    const State& aState, const Instant& anInstant, const Array<Maneuver>& aManeuverArray
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    this->validateDynamicsSet();

    const ManeuverSchedule maneuverSchedule = this->generateManeuverSchedule(aManeuverArray);

    const StateBuilder solverStateBuilder = {
        Propagator::IntegrationFrameSPtr, maneuverSchedule.maneuverCoordinateBrokerSPtr
    };

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    State solverOutputState = solverInputState;

    // Each arc is a separate call to the numerical solver: the first one must reset the observed states to guarantee
    // they only encompass this arc. The following calls to the numerical solver append to observed states.
    bool appendObservedStates = false;

    for (const ManeuverSchedule::Arc& arc : maneuverSchedule.generateArcs(solverInputState.accessInstant(), anInstant))
    {
        solverOutputState = numericalSolver_.integrateTime(
            solverOutputState,
            arc.endInstant,
            Dynamics::GetSystemOfEquations(arc.contexts, arc.startInstant, Propagator::IntegrationFrameSPtr),
            appendObservedStates
        );

        appendObservedStates = true;
    }

    const StateBuilder outputStateBuilder = {aState};

    return outputStateBuilder.expand(solverOutputState.inFrame(aState.accessFrame()), aState);
}

NumericalSolver::ConditionSolution Propagator::calculateStateToCondition(
    const State& aState,
    const Instant& anInstant,
    const EventCondition& anEventCondition,
    const Array<Maneuver>& aManeuverArray
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagator");
    }

    this->validateDynamicsSet();

    const ManeuverSchedule maneuverSchedule = this->generateManeuverSchedule(aManeuverArray);

    const Instant& startInstant = aState.accessInstant();

    const StateBuilder solverStateBuilder = {
        Propagator::IntegrationFrameSPtr, maneuverSchedule.maneuverCoordinateBrokerSPtr
    };

    const State solverInputState = solverStateBuilder.reduce(aState.inFrame(Propagator::IntegrationFrameSPtr));

    const Array<ManeuverSchedule::Arc> arcs = maneuverSchedule.generateArcs(startInstant, anInstant);

    State arcInputState = solverInputState;
    NumericalSolver::ConditionSolution conditionSolution = {solverInputState, false, 0, false};

    // Each arc is a separate call to the numerical solver: the first one must reset the observed states to guarantee
    // they only encompass this arc. The following calls to the numerical solver append to observed states.
    bool appendObservedStates = false;

    for (Index i = 0; i < arcs.getSize(); ++i)
    {
        const ManeuverSchedule::Arc& arc = arcs[i];
        const bool isLastArc = (i + 1 == arcs.getSize());

        const NumericalSolver::SystemOfEquationsWrapper arcSystemOfEquations =
            Dynamics::GetSystemOfEquations(arc.contexts, arc.startInstant, Propagator::IntegrationFrameSPtr);

        // The conditional solver lets its last step run past the end of the arc, then integrates back to the end. The
        // dynamics of an arc stop at its end (a maneuver switches off, or the next arc has different dynamics), and the
        // error estimate cannot see a step that straddles such a discontinuity. Holding the time fixed at the end of
        // the arc beyond it extends the dynamics of the arc continuously, so the overshoot and the integration back
        // stay consistent. A last arc without a maneuver has no such discontinuity, and is left untouched.
        const double arcDuration = (arc.endInstant - arc.startInstant).inSeconds();

        const NumericalSolver::SystemOfEquationsWrapper systemOfEquations =
            (isLastArc && !arc.hasManeuver)
                ? arcSystemOfEquations
                : NumericalSolver::SystemOfEquationsWrapper(
                      [arcSystemOfEquations, arcDuration](
                          const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                      )
                      {
                          const bool isForwardPropagation = arcDuration >= 0.0;
                          arcSystemOfEquations(
                              x, dxdt, isForwardPropagation ? std::min(t, arcDuration) : std::max(t, arcDuration)
                          );
                      }
                  );

        conditionSolution = numericalSolver_.integrateTime(
            arcInputState, arc.endInstant, systemOfEquations, anEventCondition, appendObservedStates
        );

        if (isLastArc)
        {
            break;
        }

        if (conditionSolution.conditionIsSatisfied)
        {
            const Instant& eventInstant = conditionSolution.state.accessInstant();
            const bool isEventWithinArc =
                arc.endInstant >= arc.startInstant ? eventInstant <= arc.endInstant : eventInstant >= arc.endInstant;

            if (isEventWithinArc)
            {
                break;
            }

            // The solver's last step can overshoot the end of the arc, and an event found there was computed with the
            // dynamics of this arc rather than those of the next one: discard it and carry on from the end of the arc.
            // The solver appending from the start of the arc also discards the states observed past it
            conditionSolution.state =
                numericalSolver_.integrateTime(arcInputState, arc.endInstant, systemOfEquations, appendObservedStates);
            conditionSolution.conditionIsSatisfied = false;
        }

        appendObservedStates = true;

        // The solver rebuilds the end instant from a floating-point duration: pin it to the exact arc boundary
        arcInputState = conditionSolution.state.accessInstant() == arc.endInstant
                          ? conditionSolution.state
                          : State(
                                arc.endInstant,
                                conditionSolution.state.accessCoordinates(),
                                conditionSolution.state.accessFrame(),
                                conditionSolution.state.accessCoordinateBroker()
                            );
    }

    const StateBuilder outputStateBuilder = {aState};

    conditionSolution.state = outputStateBuilder.expand(conditionSolution.state.inFrame(aState.accessFrame()), aState);

    return conditionSolution;
}

Array<State> Propagator::calculateStatesAt(
    const State& aState, const Array<Instant>& anInstantArray, const Array<Maneuver>& aManeuverArray
) const
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

    this->validateDynamicsSet();

    const ManeuverSchedule maneuverSchedule = this->generateManeuverSchedule(aManeuverArray);

    const StateBuilder solverStateBuilder = {
        Propagator::IntegrationFrameSPtr, maneuverSchedule.maneuverCoordinateBrokerSPtr
    };

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
    const Array<State> forwardPropagatedStates =
        this->propagateThroughManeuverSchedule(solverInputState, forwardInstants, maneuverSchedule);

    // backward propagation only
    std::reverse(backwardInstants.begin(), backwardInstants.end());

    Array<State> backwardPropagatedStates =
        this->propagateThroughManeuverSchedule(solverInputState, backwardInstants, maneuverSchedule);

    std::reverse(backwardPropagatedStates.begin(), backwardPropagatedStates.end());

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

Propagator::ManeuverSchedule Propagator::generateManeuverSchedule(const Array<Maneuver>& aManeuverArray) const
{
    if (aManeuverArray.isEmpty())
    {
        return {coordinatesBrokerSPtr_, dynamicsContexts_, Array<ManeuverSchedule::ManeuverContext>::Empty()};
    }

    // The maneuvers write to subsets (e.g. mass) that the dynamics of the propagator might not: extend a copy of the
    // broker, which leaves the existing indexes untouched, so that the propagator itself is not modified
    const Shared<CoordinateBroker> maneuverCoordinateBrokerSPtr =
        std::make_shared<CoordinateBroker>(*coordinatesBrokerSPtr_);

    Array<Interval> maneuverIntervals = Array<Interval>::Empty();
    maneuverIntervals.reserve(aManeuverArray.getSize());
    for (const Maneuver& maneuver : aManeuverArray)
    {
        maneuverIntervals.add(maneuver.getInterval());
    }

    // Overlapping, touching and duplicated maneuvers are all merged into fewer intervals
    if (Interval::Merge(maneuverIntervals).getSize() != maneuverIntervals.getSize())
    {
        throw ostk::core::error::RuntimeError("Maneuvers must be non-overlapping and unique.");
    }

    Array<ManeuverSchedule::ManeuverContext> maneuverContexts = Array<ManeuverSchedule::ManeuverContext>::Empty();
    maneuverContexts.reserve(aManeuverArray.getSize());
    for (const Maneuver& maneuver : aManeuverArray)
    {
        const Shared<Dynamics> tabulatedDynamicsSPtr = maneuver.toTabulatedDynamics(Propagator::IntegrationFrameSPtr);

        maneuverContexts.add({
            maneuver.getInterval().getStart(),
            maneuver.getInterval().getEnd(),
            Propagator::CreateContext(tabulatedDynamicsSPtr, maneuverCoordinateBrokerSPtr),
        });
    }

    // The maneuvers do not overlap, so ordering them by start instant is unambiguous
    std::sort(
        maneuverContexts.begin(),
        maneuverContexts.end(),
        [](const ManeuverSchedule::ManeuverContext& aFirstContext,
           const ManeuverSchedule::ManeuverContext& aSecondContext) -> bool
        {
            return aFirstContext.startInstant < aSecondContext.startInstant;
        }
    );

    return {maneuverCoordinateBrokerSPtr, dynamicsContexts_, maneuverContexts};
}

Array<Propagator::ManeuverSchedule::Arc> Propagator::ManeuverSchedule::generateArcs(
    const Instant& aStartInstant, const Instant& anEndInstant
) const
{
    const bool isForward = anEndInstant >= aStartInstant;
    const Instant lowerInstant = isForward ? aStartInstant : anEndInstant;
    const Instant upperInstant = isForward ? anEndInstant : aStartInstant;

    // Build the arcs in chronological order, then flip them if propagating backwards
    Array<Arc> arcs = Array<Arc>::Empty();
    Instant cursorInstant = lowerInstant;

    for (const ManeuverContext& maneuverContext : maneuvers)
    {
        if (maneuverContext.endInstant <= lowerInstant || maneuverContext.startInstant >= upperInstant)
        {
            continue;
        }

        const Instant maneuverStartInstant = std::max(maneuverContext.startInstant, lowerInstant);
        const Instant maneuverEndInstant = std::min(maneuverContext.endInstant, upperInstant);

        if (maneuverStartInstant > cursorInstant)
        {
            arcs.add({cursorInstant, maneuverStartInstant, false, freeDynamicsContexts});
        }

        Array<Dynamics::Context> arcContexts = freeDynamicsContexts;
        arcContexts.add(maneuverContext.context);
        arcs.add({maneuverStartInstant, maneuverEndInstant, true, arcContexts});

        cursorInstant = maneuverEndInstant;
    }

    if (cursorInstant < upperInstant || arcs.isEmpty())
    {
        arcs.add({cursorInstant, upperInstant, false, freeDynamicsContexts});
    }

    if (!isForward)
    {
        std::reverse(arcs.begin(), arcs.end());

        for (Arc& arc : arcs)
        {
            std::swap(arc.startInstant, arc.endInstant);
        }
    }

    return arcs;
}

Array<State> Propagator::propagateThroughManeuverSchedule(
    const State& aState, const Array<Instant>& anInstantArray, const ManeuverSchedule& aManeuverSchedule
) const
{
    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    const bool isForward = anInstantArray.accessLast() > aState.accessInstant();

    Array<State> states = Array<State>::Empty();
    states.reserve(anInstantArray.getSize());

    State arcInputState = aState;
    Index nextInstantIndex = 0;

    for (const ManeuverSchedule::Arc& arc :
         aManeuverSchedule.generateArcs(aState.accessInstant(), anInstantArray.accessLast()))
    {
        // The instants that fall within this arc (an instant on a boundary belongs to the arc that ends there)
        Array<Instant> arcInstants = Array<Instant>::Empty();
        while (nextInstantIndex < anInstantArray.getSize() &&
               (isForward ? anInstantArray[nextInstantIndex] <= arc.endInstant
                          : anInstantArray[nextInstantIndex] >= arc.endInstant))
        {
            arcInstants.add(anInstantArray[nextInstantIndex]);
            ++nextInstantIndex;
        }

        // Always integrate up to the end of the arc, which is where the next arc starts
        const bool isArcEndRequested = !arcInstants.isEmpty() && arcInstants.accessLast() == arc.endInstant;
        if (!isArcEndRequested)
        {
            arcInstants.add(arc.endInstant);
        }

        const Array<State> arcStates = numericalSolver_.integrateTime(
            arcInputState,
            arcInstants,
            Dynamics::GetSystemOfEquations(arc.contexts, arc.startInstant, Propagator::IntegrationFrameSPtr)
        );

        const Size requestedStateCount = isArcEndRequested ? arcStates.getSize() : arcStates.getSize() - 1;
        for (Index i = 0; i < requestedStateCount; ++i)
        {
            states.add(arcStates[i]);
        }

        arcInputState = arcStates.accessLast();
    }

    return states;
}

Dynamics::Context Propagator::CreateContext(
    const Shared<Dynamics>& aDynamicsSPtr, const Shared<CoordinateBroker>& aCoordinateBrokerSPtr
)
{
    // Store read coordinate subsets information
    Array<Pair<Index, Size>> readInfo = Array<Pair<Index, Size>>::Empty();
    readInfo.reserve(aDynamicsSPtr->getReadCoordinateSubsets().getSize());
    for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getReadCoordinateSubsets())
    {
        const Pair<Index, Size> indexAndSize = {aCoordinateBrokerSPtr->addSubset(subset), subset->getSize()};
        readInfo.add(indexAndSize);
    }

    // Store write coordinate subsets information
    Array<Pair<Index, Size>> writeInfo = Array<Pair<Index, Size>>::Empty();
    writeInfo.reserve(aDynamicsSPtr->getWriteCoordinateSubsets().getSize());
    for (const Shared<const CoordinateSubset>& subset : aDynamicsSPtr->getWriteCoordinateSubsets())
    {
        const Pair<Index, Size> indexAndSize = {aCoordinateBrokerSPtr->addSubset(subset), subset->getSize()};
        writeInfo.add(indexAndSize);
    }

    return {aDynamicsSPtr, readInfo, writeInfo};
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
