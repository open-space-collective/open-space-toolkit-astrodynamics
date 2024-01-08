/// Apache License 2.0

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::physics::time::Duration;

using ostk::astro::trajectory::orbit::models::Tabulated;

const Instant& Sequence::Solution::accessStartInstant() const
{
    return segmentSolutions.accessFirst().accessStartInstant();
}

const Instant& Sequence::Solution::accessEndInstant() const
{
    return segmentSolutions.accessLast().accessEndInstant();
}

Array<State> Sequence::Solution::getStates() const
{
    if (segmentSolutions.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Segment solutions are empty.");
    }

    Array<State> states = segmentSolutions.accessFirst().states;

    for (Index i = 1; i < segmentSolutions.getSize(); ++i)
    {
        const Array<State>& segmentStates =
            Array<State>(segmentSolutions[i].states.begin() + 1, segmentSolutions[i].states.end());

        states.add(segmentStates);
    }

    return states;
}

Array<State> Sequence::Solution::getStatesAt(const NumericalSolver& numericalSolver, const Array<Instant>& instants)
    const
{
    if (segmentSolutions.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Segment solutions are empty.");
    }

    Array<State> states = Array<State>::Empty();

    for (const auto& segmentSolution : segmentSolutions)
    {
        states.add(segmentSolution.getStatesAt(numericalSolver, instants));
    }

    return states;
}

Array<State> Sequence::Solution::approximateStatesAt(const Array<Instant>& instants) const
{
    if (segmentSolutions.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Segment solutions are empty.");
    }

    const Array<State> states = getStates();

    const Tabulated tabulated = {states, 0, Tabulated::InterpolationType::BarycentricRational};

    return tabulated.calculateStatesAt(instants);
}

Mass Sequence::Solution::getInitialMass() const
{
    return segmentSolutions.accessFirst().getInitialMass();
}

Mass Sequence::Solution::getFinalMass() const
{
    return segmentSolutions.accessLast().getFinalMass();
}

Duration Sequence::Solution::getPropagationDuration() const
{
    return accessEndInstant() - accessStartInstant();
}

Mass Sequence::Solution::computeDeltaMass() const
{
    return Mass::Kilograms(getInitialMass().inKilograms() - getFinalMass().inKilograms());
}

Real Sequence::Solution::computeDeltaV(const Real& aSpecificImpulse) const
{
    Real deltaV = 0.0;

    for (const auto& segmentSolution : segmentSolutions)
    {
        deltaV += segmentSolution.computeDeltaV(aSpecificImpulse);
    }

    return deltaV;
}

void Sequence::Solution::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Sequence Solution");
    }

    ostk::core::utils::Print::Line(anOutputStream)
        << "Execution is complete: " << (executionIsComplete ? "True" : "False");

    ostk::core::utils::Print::Separator(anOutputStream, "Segment Solutions");

    bool hasManeuver = false;
    for (const auto& segmentSolution : segmentSolutions)
    {
        segmentSolution.print(anOutputStream, false);

        if (segmentSolution.segmentType == Segment::Type::Maneuver)
        {
            hasManeuver = true;
        }

        ostk::core::utils::Print::Separator(anOutputStream);
    }

    ostk::core::utils::Print::Separator(anOutputStream, "Summary");

    ostk::core::utils::Print::Line(anOutputStream) << "Start Instant: " << accessStartInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "End Instant: " << accessEndInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Propagation Duration: " << getPropagationDuration().toString();

    if (hasManeuver)
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Initial Mass: " << getInitialMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Final Mass: " << getFinalMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Total Mass consumed: " << computeDeltaMass().toString();
    }

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const Sequence::Solution& aSolution)
{
    aSolution.print(anOutputStream);

    return anOutputStream;
}

Sequence::Sequence(
    const Array<Segment>& aSegmentArray,
    const NumericalSolver& aNumericalSolver,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Duration& maximumPropagationDuration,
    const Size& verbosity
)
    : segments_(aSegmentArray),
      numericalSolver_(aNumericalSolver),
      dynamics_(aDynamicsArray),
      segmentPropagationDurationLimit_(maximumPropagationDuration)
{
    if (verbosity == 5)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
    }
    else if (verbosity == 4)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
    }
    else if (verbosity == 3)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
    }
    else if (verbosity == 2)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
    }
    else if (verbosity == 1)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
    }
    else if (verbosity == 0)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::fatal);
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("verbosity level");
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
    return segmentPropagationDurationLimit_;
}

void Sequence::addSegment(const Segment& aSegment)
{
    segments_.add(aSegment);
}

void Sequence::addSegments(const Array<Segment>& aTrajectorySegmentArray)
{
    segments_.add(aTrajectorySegmentArray);
}

void Sequence::addCoastSegment(const Shared<EventCondition>& anEventConditionSPtr)
{
    segments_.add(Segment::Coast("Coast", anEventConditionSPtr, dynamics_, numericalSolver_));
}

void Sequence::addManeuverSegment(const Shared<EventCondition>& anEventConditionSPtr, const Shared<Thruster>& aThruster)
{
    segments_.add(Segment::Maneuver("Maneuver", anEventConditionSPtr, aThruster, dynamics_, numericalSolver_));
}

Sequence::Solution Sequence::solve(const State& aState, const Size& aRepetitionCount) const
{
    if (aRepetitionCount <= 0)
    {
        throw ostk::core::error::runtime::Wrong("Repetition count.");
    }

    Array<Segment::Solution> segmentSolutions;

    State initialState = aState;
    State finalState = State::Undefined();

    for (Size i = 0; i < aRepetitionCount; ++i)
    {
        for (const Segment& segment : segments_)
        {
            segment.accessEventCondition()->updateTarget(initialState);

            BOOST_LOG_TRIVIAL(debug) << "Solving Segment:\n" << segment << std::endl;

            Segment::Solution segmentSolution = segment.solve(initialState, segmentPropagationDurationLimit_);

            segmentSolution.name =
                String::Format("{} - {} - {}", segmentSolution.name, segment.getEventCondition()->getName(), i);

            BOOST_LOG_TRIVIAL(debug) << "\n" << segmentSolution << std::endl;

            segmentSolutions.add(segmentSolution);

            // Terminate Sequence unsuccessfully if the segment condition was not satisfied
            if (!segmentSolution.conditionIsSatisfied)
            {
                BOOST_LOG_TRIVIAL(warning) << "Segment condition is not satisfied." << std::endl;

                return {segmentSolutions, false};
            }

            initialState = segmentSolution.states.accessLast();
        }
    }

    return {segmentSolutions, true};
}

Sequence::Solution Sequence::solveToCondition(
    const State& aState, const EventCondition& anEventCondition, const Duration& aMaximumPropagationDuration
) const
{
    Array<Segment::Solution> segmentSolutions;

    State initialState = aState;
    State finalState = State::Undefined();

    bool eventConditionIsSatisfied = false;

    Duration propagationDuration = Duration::Zero();

    while (!eventConditionIsSatisfied && propagationDuration <= aMaximumPropagationDuration)
    {
        for (const Segment& segment : segments_)
        {
            segment.accessEventCondition()->updateTarget(initialState);

            BOOST_LOG_TRIVIAL(debug) << "Solving Segment:\n" << segment << std::endl;

            const Duration segmentPropagationDurationLimit =
                std::min(segmentPropagationDurationLimit_, aMaximumPropagationDuration - propagationDuration);

            Segment::Solution segmentSolution = segment.solve(initialState, segmentPropagationDurationLimit);

            segmentSolution.name =
                String::Format("{} - {}", segmentSolution.name, segment.getEventCondition()->getName());

            BOOST_LOG_TRIVIAL(debug) << "\n" << segmentSolution << std::endl;

            segmentSolutions.add(segmentSolution);

            // Terminate Sequence unsuccessfully if the segment condition was not satisfied
            if (!segmentSolution.conditionIsSatisfied)
            {
                BOOST_LOG_TRIVIAL(warning) << "Segment condition is not satisfied." << std::endl;

                return {segmentSolutions, false};
            }

            eventConditionIsSatisfied = anEventCondition.isSatisfied(segmentSolution.states.accessLast(), initialState);

            // Terminate Sequence successfully if a provided event condition was satisfied
            if (eventConditionIsSatisfied)
            {
                return {segmentSolutions, true};
            }

            propagationDuration += segmentSolution.getPropagationDuration();

            initialState = segmentSolution.states.accessLast();
        }
    }

    return {segmentSolutions, false};
}

void Sequence::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Sequence");
    }

    ostk::core::utils::Print::Separator(anOutputStream, "Segments");
    Index iter = 0;
    for (const auto& segment : segments_)
    {
        ostk::core::utils::Print::Separator(anOutputStream, String::Format("Segment {}", iter));
        segment.print(anOutputStream, false);
        ostk::core::utils::Print::Separator(anOutputStream);

        ++iter;
    }
    ostk::core::utils::Print::Line(anOutputStream);

    ostk::core::utils::Print::Separator(anOutputStream, "Default Numerical Solver");
    numericalSolver_.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Common Dynamics");
    for (const auto& dynamics : dynamics_)
    {
        dynamics->print(anOutputStream, false);
    }

    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum Propagation Duration:" << segmentPropagationDurationLimit_.toString();

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
