/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/TrajectorySegment.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::physics::time::Interval;
using ostk::physics::time::Duration;

using ostk::astro::trajectory::Propagator;
using ostk::astro::eventcondition::DurationCondition;
using ostk::astro::trajectory::models::Tabulated;

TrajectorySegment::Solution::Solution(
    const String& aName, const Array<Shared<Dynamics>>& aDynamicsArray, const Array<State>& aStates
)
    : name(aName),
      dynamics(aDynamicsArray),
      states(aStates)
{
}

TrajectorySegment::TrajectorySegment(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
    : name_(aName),
      eventCondition_(anEventConditionSPtr),
      dynamics_(aDynamicsArray),
      numericalSolver_(aNumericalSolver)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const TrajectorySegment& aSegment)
{
    aSegment.print(anOutputStream, false);

    return anOutputStream;
}

String TrajectorySegment::getName() const
{
    return name_;
}

Shared<EventCondition> TrajectorySegment::getEventCondition() const
{
    return eventCondition_;
}

Array<Shared<Dynamics>> TrajectorySegment::getDynamics() const
{
    return dynamics_;
}

NumericalSolver TrajectorySegment::getNumericalSolver() const
{
    return numericalSolver_;
}

TrajectorySegment::Solution TrajectorySegment::solve(
    const State& aState,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver,
    const Duration& maximumPropagationDuration
) const
{
    const NumericalSolver numericalSolver = aNumericalSolver.isDefined() ? aNumericalSolver : numericalSolver_;

    if (!numericalSolver.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Numerical Solver");
    }

    const Array<Shared<Dynamics>> dynamics = dynamics_ + aDynamicsArray;

    if (dynamics.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Dynamics");
    }

    const Propagator propagator = {
        numericalSolver,
        dynamics,
    };

    const Instant startInstant = aState.getInstant();

    // TBI: Handle the case where the condition is not met
    const State finalState =
        propagator.calculateStateAt(aState, startInstant + maximumPropagationDuration, *eventCondition_);

    Array<State> propagatedStates = Array<State>::Empty();
    propagatedStates.reserve(propagator.accessNumericalSolver().getObservedStates().getSize());

    for (const auto& solution : propagator.accessNumericalSolver().getObservedStates())
    {
        propagatedStates.add({
            startInstant + Duration::Seconds(solution.second),
            solution.first,
            Frame::GCRF(),  // TBM: Use the frame the propagator
            propagator.accessCoordinatesBroker(),
        });
    }

    return {
        name_,
        dynamics,
        propagatedStates,
    };
}

void TrajectorySegment::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "TrajectorySegment");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    eventCondition_->print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << dynamics_;
    ostk::core::utils::Print::Line(anOutputStream) << numericalSolver_;

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

TrajectorySegment TrajectorySegment::Coast(
    const Shared<EventCondition>& anEventConditionSPtr,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
{
    return {
        "Coast",
        anEventConditionSPtr,
        aDynamicsArray,
        aNumericalSolver,
    };
}

TrajectorySegment TrajectorySegment::Maneuver(
    const Shared<EventCondition>& anEventConditionSPtr,
    const Shared<Dynamics>& aThrusterDynamics,
    const NumericalSolver& aNumericalSolver
)
{
    (void)anEventConditionSPtr;
    (void)aThrusterDynamics;
    (void)aNumericalSolver;

    throw ostk::core::error::runtime::ToBeImplemented(
        "TrajectorySegment: Maneuver(EventCondition, Shared<Dynamics>, NumericalSolver)"
    );
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
