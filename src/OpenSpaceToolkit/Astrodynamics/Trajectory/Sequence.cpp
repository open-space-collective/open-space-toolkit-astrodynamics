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
    if (segments_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Segments");
    }
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

Sequence::Solution Sequence::solve(const State& aState) const
{
    Array<Segment::Solution> segmentSolutions;

    State initialState = aState;
    State finalState = State::Undefined();

    for (Size i = 0; i < repetitionCount_; ++i)
    {
        for (const Segment& segment : segments_)
        {
            Segment::Solution segmentSolution =
                segment.solve(initialState, dynamics_, numericalSolver_, maximumPropagationDuration_);

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
