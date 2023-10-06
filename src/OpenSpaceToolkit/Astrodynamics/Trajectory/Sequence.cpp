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

    for (const Segment::Solution& segmentSolution : segmentSolutions)
    {
        states.add(segmentSolution.states);
    }

    return states;
}

Sequence::Sequence(
    const Array<Segment>& aSegmentArray,
    const Size& aRepetitionCount,
    const NumericalSolver& aNumericalSolver,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Duration& maximumPropagationDuration
)
    : segments_(aSegmentArray),
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

Array<Segment> Sequence::getSegments() const
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

void Sequence::addSegment(const Segment& aSegment)
{
    segments_.add(aSegment);
}

void Sequence::addSegment(const Array<Segment>& someSegments)
{
    segments_.add(someSegments);
}

void Sequence::addCoastSegment(const Shared<EventCondition>& anEventConditionSPtr)
{
    segments_.add(Segment::Coast("Coast", anEventConditionSPtr, dynamics_, numericalSolver_));
}

void Sequence::addManeuverSegment(const Shared<EventCondition>& anEventConditionSPtr, const Shared<Thruster>& aThruster)
{
    segments_.add(Segment::Maneuver("Maneuver", anEventConditionSPtr, aThruster, dynamics_, numericalSolver_));
}

Sequence::Solution Sequence::solve(const State& aState) const
{
    Array<Segment::Solution> segmentSolutions;

    State initialState = aState;
    State finalState = State::Undefined();

    for (Size i = 0; i < repetitionCount_; ++i)
    {
        for (const Segment& segment : segments_)
        {
            Segment::Solution segmentSolution = segment.solve(initialState, maximumPropagationDuration_);

            segmentSolution.name =
                String::Format("{} - {} - {}", segmentSolution.name, segment.getEventCondition()->getName(), i);

            segmentSolutions.add(segmentSolution);

            if (!segmentSolution.conditionIsSatisfied)
            {
                return {segmentSolutions, false};
            }

            initialState = segmentSolution.states.accessLast();
        }
    }

    return {segmentSolutions, true};
}

void Sequence::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Sequence");
    }

    ostk::core::utils::Print::Separator(anOutputStream, "Segments");
    ostk::core::utils::Print::Line(anOutputStream) << segments_;
    ostk::core::utils::Print::Separator(anOutputStream, "Default Numerical Solver");
    ostk::core::utils::Print::Line(anOutputStream) << numericalSolver_;
    ostk::core::utils::Print::Separator(anOutputStream, "Common Dynamics");
    ostk::core::utils::Print::Line(anOutputStream) << dynamics_;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum Propagation Duration:" << maximumPropagationDuration_.toString();

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
