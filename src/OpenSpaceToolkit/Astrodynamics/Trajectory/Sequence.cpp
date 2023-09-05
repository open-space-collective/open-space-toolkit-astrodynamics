/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

Array<State> Sequence::Solution::getStates() const
{
    Array<State> states = Array<State>::Empty();

    for (const TrajectorySegment::Solution& segmentSolution : segmentSolutions)
    {
        states.add(segmentSolution.states);
    }

    return states;
}

Sequence::Sequence(
    const Size& aRepetitionCount,
    const NumericalSolver& aNumericalSolver,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Duration& maximumPropagationDuration
)
    : segments_(Array<TrajectorySegment>::Empty()),
      repetitionCount_(aRepetitionCount),
      numericalSolver_(aNumericalSolver),
      dynamics_(aDynamicsArray),
      maximumPropagationDuration_(maximumPropagationDuration)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const Sequence& aSequence)
{
    aSequence.print(anOutputStream, false);

    return anOutputStream;
}

Array<TrajectorySegment> Sequence::getSegments() const
{
    return segments_;
}

NumericalSolver Sequence::getNumericalSolver() const
{
    return numericalSolver_;
}

Array<Shared<Dynamics>> Sequence::getDynamics() const
{
    return dynamics_;
}

Duration Sequence::getMaximumPropagationDuration() const
{
    return maximumPropagationDuration_;
}

void Sequence::addSegment(const TrajectorySegment& aSegment)
{
    segments_.add(aSegment);
}

void Sequence::addSegment(const Array<TrajectorySegment>& someSegments)
{
    segments_.add(someSegments);
}

void Sequence::addCoastSegment(const Shared<EventCondition>& anEventConditionSPtr)
{
    segments_.add(TrajectorySegment::Coast("Coast", anEventConditionSPtr, dynamics_, numericalSolver_));
}

void Sequence::addManeuverSegment(const Shared<EventCondition>& anEventConditionSPtr, const Shared<Dynamics>& aThruster)
{
    segments_.add(TrajectorySegment::Maneuver("Maneuver", anEventConditionSPtr, aThruster, dynamics_, numericalSolver_)
    );
}

Sequence::Solution Sequence::solve(const State& aState) const
{
    Array<TrajectorySegment::Solution> segmentSolutions;

    State initialState = aState;
    State finalState = State::Undefined();

    std::cout << "Sequence::solve: " << initialState << std::endl;

    for (Size i = 0; i < repetitionCount_; ++i)
    {
        for (const TrajectorySegment& segment : segments_)
        {
            std::cout << "Sequence::solve: " << i << ": " << segment << std::endl;
            TrajectorySegment::Solution segmentSolution = segment.solve(initialState, maximumPropagationDuration_);

            std::cout << "solved" << std::endl;
            segmentSolution.name =
                String::Format("{} - {} - {}", segmentSolution.name, segment.getEventCondition()->getName(), i);

            segmentSolutions.add(segmentSolution);

            initialState = segmentSolution.states.accessLast();
        }
    }

    return {segmentSolutions};
}

void Sequence::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Sequence");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Segments:" << segments_;
    ostk::core::utils::Print::Line(anOutputStream) << "Default Numerical Solver:" << numericalSolver_;
    ostk::core::utils::Print::Line(anOutputStream) << "Common Dynamics:" << dynamics_;
    ostk::core::utils::Print::Line(anOutputStream) << "Maximum Propagation Duration:" << maximumPropagationDuration_;

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
