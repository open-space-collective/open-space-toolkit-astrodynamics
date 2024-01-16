/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::physics::time::Duration;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

Segment::Solution::Solution(
    const String& aName,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Array<State>& aStates,
    const bool& aConditionIsSatisfied,
    const Segment::Type& aSegmentType
)
    : name(aName),
      dynamics(aDynamicsArray),
      states(aStates),
      conditionIsSatisfied(aConditionIsSatisfied),
      segmentType(aSegmentType)
{
}

const Instant& Segment::Solution::accessStartInstant() const
{
    if (states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return states.accessFirst().accessInstant();
}

const Instant& Segment::Solution::accessEndInstant() const
{
    if (states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return states.accessLast().accessInstant();
}

Mass Segment::Solution::getInitialMass() const
{
    if (states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return Mass::Kilograms(states.accessFirst().extractCoordinate(CoordinatesSubset::Mass())[0]);
}

Mass Segment::Solution::getFinalMass() const
{
    if (states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return Mass::Kilograms(states.accessLast().extractCoordinate(CoordinatesSubset::Mass())[0]);
}

Duration Segment::Solution::getPropagationDuration() const
{
    return accessEndInstant() - accessStartInstant();
}

Real Segment::Solution::computeDeltaV(const Real& aSpecificImpulse) const
{
    // TBM: This is only valid for constant thrust, constant Isp
    if (segmentType != Segment::Type::Maneuver)
    {
        return 0.0;
    }

    return aSpecificImpulse * EarthGravitationalModel::gravityConstant *
           std::log(getInitialMass().inKilograms() / getFinalMass().inKilograms());
}

Mass Segment::Solution::computeDeltaMass() const
{
    if (segmentType != Segment::Type::Maneuver)
    {
        return Mass::Kilograms(0.0);
    }

    return Mass::Kilograms(getInitialMass().inKilograms() - getFinalMass().inKilograms());
}

Array<VectorXd> Segment::Solution::getDynamicsContributions(
    const Shared<Dynamics>& aDynamicsSPtr,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetSPtrArray
) const
{
    // Check dynamics is part of the segment dynamics
    if (!dynamics.contains(aDynamicsSPtr))
    {
        throw ostk::core::error::RuntimeError("Provided dynamics is not part of the segment dynamics.");
    }

    // Extract write coordinates subsets from dynamics
    const Array<Shared<const CoordinatesSubset>> dynamicsWriteCoordinatesSubsets =
        aDynamicsSPtr->getWriteCoordinatesSubsets();

    // Check that the provided coordinates subsets are part of the dynamics write coordinates subsets
    for (auto aCoordinatesSubsetSPtr : aCoordinatesSubsetSPtrArray)
    {
        if (!dynamicsWriteCoordinatesSubsets.contains(aCoordinatesSubsetSPtr))
        {
            throw ostk::core::error::RuntimeError(String::Format(
                "Provided coordinates subset [{}] is not part of the dynamics write coordinates subsets.",
                aCoordinatesSubsetSPtr->getName()
            ));
        }
    }

    // Initialize the definitive coordinate subset array
    Array<Shared<const CoordinatesSubset>> definitiveCoordinateSubsetArray = aCoordinatesSubsetSPtrArray;

    // Check value for aCoordinatesSubsetSPtrArray
    if (aCoordinatesSubsetSPtrArray.isEmpty())
    {
        definitiveCoordinateSubsetArray = aDynamicsSPtr->getWriteCoordinatesSubsets();
    }

    // Extract states size
    const Size numberOfstates = this->states.getSize();

    // Extract dynamics context and behavior relative to state
    Array<Shared<const CoordinatesSubset>> dynamicsReadCoordinatesSubsets = aDynamicsSPtr->getReadCoordinatesSubsets();

    // Construct state builder
    const StateBuilder builder = StateBuilder(aFrameSPtr, dynamicsReadCoordinatesSubsets);

    Array<VectorXd> dynamicsContributions = Array<VectorXd>::Empty();

    // Construct the dynamicsContributions, state by state
    for (Index stateIndex = 0; stateIndex < numberOfstates; ++stateIndex)
    {
        const State& state = states[stateIndex];

        VectorXd dynamicsContributionAtState = aDynamicsSPtr->computeContribution(
            state.getInstant(), builder.reduce(state.inFrame(aFrameSPtr)).getCoordinates(), aFrameSPtr
        );
        dynamicsContributions.add(dynamicsContributionAtState);
    }

    return dynamicsContributions;
}

Array<Vector3d> Segment::Solution::getDynamicsAccelerationContributions(
    const Shared<Dynamics>& aDynamicsSPtr, const Shared<const Frame>& aFrameSPtr
) const
{
    const Array<VectorXd> dynamicsAccelerationContributions =
        this->getDynamicsContributions(aDynamicsSPtr, aFrameSPtr, {CartesianVelocity::Default()});

    Array<Vector3d> accelerationContributions = Array<Vector3d>::Empty();
    for (const VectorXd& dynamicsAccelerationContribution : dynamicsAccelerationContributions)
    {
        accelerationContributions.add(dynamicsAccelerationContribution.head<3>());
    }

    return accelerationContributions;
}

Array<Map<Shared<Dynamics>, VectorXd>> Segment::Solution::getAllDynamicsContributions(
    const Shared<const Frame>& aFrameSPtr
) const
{
    // TBI: Use smart caching for multiple calls in the future

    Map<Shared<Dynamics>, Array<VectorXd>> dynamicsContributionsMap = Map<Shared<Dynamics>, Array<VectorXd>>();

    // Create map for each dynamics
    for (const Shared<Dynamics>& aDynamicsSPtr : this->dynamics)
    {
        Array<VectorXd> dynamicsContributions = this->getDynamicsContributions(aDynamicsSPtr, aFrameSPtr);
        dynamicsContributionsMap.emplace(aDynamicsSPtr, dynamicsContributions);
    }

    Array<Map<Shared<Dynamics>, VectorXd>> dynamicsContributionsMaps = Array<Map<Shared<Dynamics>, VectorXd>>::Empty();

    const Size numberOfstates = this->states.getSize();
    for (Index stateIndex = 0; stateIndex < numberOfstates; ++stateIndex)
    {
        // Create map for each state and fill it with the dynamics contributions
        Map<Shared<Dynamics>, VectorXd> dynamicsContributionMapAtState = Map<Shared<Dynamics>, VectorXd>();
        for (const Shared<Dynamics>& aDynamicsSPtr : this->dynamics)
        {
            dynamicsContributionMapAtState.emplace(aDynamicsSPtr, dynamicsContributionsMap[aDynamicsSPtr][stateIndex]);
        }
        dynamicsContributionsMaps.add(dynamicsContributionMapAtState);
    }

    return dynamicsContributionsMaps;
}

void Segment::Solution::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Segment Solution");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Condition satisfied:" << (conditionIsSatisfied ? "True" : "False");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Segment type:" << (segmentType == Segment::Type::Coast ? "Coast" : "Maneuver");

    ostk::core::utils::Print::Line(anOutputStream) << "Start instant:" << accessStartInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "End instant:" << accessEndInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Propagation duration:" << (accessEndInstant() - accessStartInstant()).toString();

    if (segmentType == Segment::Type::Maneuver)
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Initial mass:" << getInitialMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Final mass:" << getFinalMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Total mass consumed:" << computeDeltaMass().toString();
    }

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const Segment::Solution& aSolution)
{
    aSolution.print(anOutputStream);

    return anOutputStream;
}

Segment::Segment(
    const String& aName,
    const Segment::Type& aType,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
    : name_(aName),
      type_(aType),
      eventCondition_(anEventConditionSPtr),
      dynamics_(aDynamicsArray),
      numericalSolver_(aNumericalSolver)
{
    if (eventCondition_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Event condition");
    }

    if (dynamics_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    if (!numericalSolver_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Numerical solver");
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const Segment& aSegment)
{
    aSegment.print(anOutputStream);

    return anOutputStream;
}

String Segment::getName() const
{
    return name_;
}

Shared<EventCondition> Segment::getEventCondition() const
{
    return accessEventCondition();
}

Array<Shared<Dynamics>> Segment::getDynamics() const
{
    return accessDynamics();
}

NumericalSolver Segment::getNumericalSolver() const
{
    return accessNumericalSolver();
}

Segment::Type Segment::getType() const
{
    return type_;
}

const Shared<EventCondition>& Segment::accessEventCondition() const
{
    return eventCondition_;
}

const Array<Shared<Dynamics>>& Segment::accessDynamics() const
{
    return dynamics_;
}

const NumericalSolver& Segment::accessNumericalSolver() const
{
    return numericalSolver_;
}

Segment::Solution Segment::solve(const State& aState, const Duration& maximumPropagationDuration) const
{
    const Propagator propagator = {
        numericalSolver_,
        dynamics_,
    };

    const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
        aState, aState.accessInstant() + maximumPropagationDuration, *eventCondition_
    );

    return {
        name_,
        dynamics_,
        propagator.accessNumericalSolver().accessObservedStates(),
        conditionSolution.conditionIsSatisfied,
        type_,
    };
}

void Segment::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Segment");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << (type_ == Segment::Type::Coast ? "Coast" : "Maneuver");
    ostk::core::utils::Print::Separator(anOutputStream, "Event Condition");
    eventCondition_->print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream);

    ostk::core::utils::Print::Separator(anOutputStream, "Dynamics");
    for (const auto& dynamics : dynamics_)
    {
        dynamics->print(anOutputStream, false);
    }
    ostk::core::utils::Print::Line(anOutputStream);

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver");
    numericalSolver_.print(anOutputStream, false);

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

Segment Segment::Coast(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
{
    return {
        aName,
        Segment::Type::Coast,
        anEventConditionSPtr,
        aDynamicsArray,
        aNumericalSolver,
    };
}

Segment Segment::Maneuver(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Shared<Thruster>& aThrusterDynamics,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
{
    return {
        aName,
        Segment::Type::Maneuver,
        anEventConditionSPtr,
        aDynamicsArray + Array<Shared<Dynamics>> {aThrusterDynamics},
        aNumericalSolver,
    };
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
