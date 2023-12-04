/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

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

Array<MatrixXd> Segment::Solution::computeAccelerations() const
{
    // Each MatrixXd contains the acceleration for a single dynamics for all the segment states
    // TBI: Return contributions in the same order as the dynamics, might want to standardize later OR use a map with
    // dynamics name (but need to make sure they are unique and existing)
    Array<MatrixXd> accelerations = Array<MatrixXd>::Empty();
    Size stateSize = states.getSize();

    for (const auto& singleDynamics : dynamics)
    {
        // Extract dynamics context and behavior relative to state
        Array<Shared<const CoordinatesSubset>> dynamicsReadCoordinatesSubsets =
            singleDynamics->getReadCoordinatesSubsets();
        Array<Shared<const CoordinatesSubset>> dynamicsWriteCoordinatesSubsets =
            singleDynamics->getWriteCoordinatesSubsets();

        // Initialize the singleDynamicsAcceleration matrix
        // Each row corresponds to a state, each column to a coordinate of the (full or reduced?) State
        // TBI: Decide if all contributions should be homogenized already
        Size dynamicsWriteSize = std::accumulate(
            dynamicsWriteCoordinatesSubsets.begin(),
            dynamicsWriteCoordinatesSubsets.end(),
            0,
            [](int sum, const Shared<const CoordinatesSubset>& subset)
            {
                return sum + subset->getSize();
            }
        );
        MatrixXd singleDynamicsAcceleration = MatrixXd::Zero(stateSize, dynamicsWriteSize);

        // Construct the singleDynamicsAcceleration matrix, state by state
        for (Index stateIndex = 0; stateIndex < stateSize; ++stateIndex)
        {
            State state = states[stateIndex];

            Shared<const CoordinatesBroker> coordinatesBroker = state.accessCoordinatesBroker();

            VectorXd readStateCoordinates =
                coordinatesBroker->extractCoordinates(state.getCoordinates(), dynamicsReadCoordinatesSubsets);
            VectorXd writeStateCoordinates =
                coordinatesBroker->extractCoordinates(state.getCoordinates(), dynamicsWriteCoordinatesSubsets);

            VectorXd accelerationContributionAtState = singleDynamics->computeContribution(
                state.getInstant(),
                readStateCoordinates,
                Frame::GCRF()  // TBI: Make more general here
            );

            singleDynamicsAcceleration.row(stateIndex) = accelerationContributionAtState;
        }

        accelerations.add(singleDynamicsAcceleration);
    }

    // TBI: Cache at solution determination time?

    return accelerations;
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
