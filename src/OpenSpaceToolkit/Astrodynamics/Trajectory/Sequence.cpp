/// Apache License 2.0

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::guidancelaw::HeterogeneousGuidanceLaw;
using ostk::core::type::Unique;
using ostk::physics::time::Duration;

Sequence::Solution::Solution(const Array<Segment::Solution>& aSegmentSolutionArray, const bool& anExecutionIsComplete)
    : segmentSolutions(aSegmentSolutionArray),
      executionIsComplete(anExecutionIsComplete)
{
}

const Instant& Sequence::Solution::accessStartInstant() const
{
    return this->segmentSolutions.accessFirst().accessStartInstant();
}

const Instant& Sequence::Solution::accessEndInstant() const
{
    return this->segmentSolutions.accessLast().accessEndInstant();
}

Interval Sequence::Solution::getInterval() const
{
    return Interval::Closed(this->accessStartInstant(), this->accessEndInstant());
}

Array<State> Sequence::Solution::getStates() const
{
    if (this->segmentSolutions.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Segment solutions are empty.");
    }

    Array<State> states = this->segmentSolutions.accessFirst().states;

    for (Index i = 1; i < this->segmentSolutions.getSize(); ++i)
    {
        const Array<State>& segmentStates =
            Array<State>(this->segmentSolutions[i].states.begin() + 1, this->segmentSolutions[i].states.end());

        states.add(segmentStates);
    }

    return states;
}

Mass Sequence::Solution::getInitialMass() const
{
    return this->segmentSolutions.accessFirst().getInitialMass();
}

Mass Sequence::Solution::getFinalMass() const
{
    return this->segmentSolutions.accessLast().getFinalMass();
}

Duration Sequence::Solution::getPropagationDuration() const
{
    return this->accessEndInstant() - this->accessStartInstant();
}

Real Sequence::Solution::computeDeltaV(const Real& aSpecificImpulse) const
{
    Real deltaV = 0.0;

    for (const auto& segmentSolution : this->segmentSolutions)
    {
        deltaV += segmentSolution.computeDeltaV(aSpecificImpulse);
    }

    return deltaV;
}

Mass Sequence::Solution::computeDeltaMass() const
{
    return Mass::Kilograms(getInitialMass().inKilograms() - getFinalMass().inKilograms());
}

Array<Maneuver> Sequence::Solution::extractManeuvers(const Shared<const Frame>& aFrameSPtr) const
{
    Array<Maneuver> maneuvers = Array<Maneuver>::Empty();

    for (const Segment::Solution& segmentSolution : this->segmentSolutions)
    {
        const Array<Maneuver> segmentManeuvers = segmentSolution.extractManeuvers(aFrameSPtr);
        maneuvers.add(segmentManeuvers);
    }

    return maneuvers;
}

Array<State> Sequence::Solution::calculateStatesAt(
    const Array<Instant>& anInstantArray, const NumericalSolver& aNumericalSolver
) const
{
    if (this->segmentSolutions.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Segment solutions are empty.");
    }

    Array<State> intermediateStates = Array<State>::Empty();

    // Process the instant array so that it is done segment by segment
    for (Size i = 0; i < this->segmentSolutions.getSize(); i++)
    {
        const Segment::Solution& segmentSolution = this->segmentSolutions.at(i);
        // Filter instants to be within segment bounds
        Array<Instant> segmentInstants = Array<Instant>::Empty();

        for (const Instant& instant : anInstantArray)
        {
            if ((instant >= segmentSolution.accessStartInstant()) && (instant < segmentSolution.accessEndInstant()))
            {
                segmentInstants.add(instant);
            }
            else if ((i == this->segmentSolutions.getSize() - 1) && (instant == this->accessEndInstant()))
            {
                segmentInstants.add(instant);
            }
        }
        intermediateStates.add(segmentSolution.calculateStatesAt(segmentInstants, aNumericalSolver));
    }

    return intermediateStates;
}

void Sequence::Solution::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Sequence Solution");
    }

    ostk::core::utils::Print::Line(anOutputStream)
        << "Execution is complete: " << (this->executionIsComplete ? "True" : "False");

    ostk::core::utils::Print::Separator(anOutputStream, "Segment Solutions");

    bool hasManeuver = false;
    for (const auto& segmentSolution : this->segmentSolutions)
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
    const Duration& aMaximumPropagationDuration,
    const Duration& aMinimumManeuverDuration,
    const Size& aVerbosityLevel
)
    : segments_(aSegmentArray),
      numericalSolver_(aNumericalSolver),
      dynamics_(aDynamicsArray),
      segmentPropagationDurationLimit_(aMaximumPropagationDuration),
      minimumManeuverDuration_(Duration::Undefined()),
      minimumManeuverSeparation_(Duration::Seconds(30.0)),
      maximumManeuverDuration_(Duration::Undefined())
{
    if (!aMaximumPropagationDuration.isDefined())
    {
        throw ostk::core::error::runtime::Wrong("Maximum propagation duration must be defined.");
    }

    if (!aMaximumPropagationDuration.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Maximum propagation duration must be strictly positive.");
    }

    setMinimumManeuverDuration(aMinimumManeuverDuration);

    if (aVerbosityLevel == 5)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
    }
    else if (aVerbosityLevel == 4)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
    }
    else if (aVerbosityLevel == 3)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
    }
    else if (aVerbosityLevel == 2)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
    }
    else if (aVerbosityLevel == 1)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
    }
    else if (aVerbosityLevel == 0)
    {
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::fatal);
    }
    else
    {
        throw ostk::core::error::runtime::Wrong(
            "verbosity level", String::Format("{}", static_cast<int>(aVerbosityLevel))
        );
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

Duration Sequence::getMaximumManeuverDuration() const
{
    return maximumManeuverDuration_;
}

Duration Sequence::getMaximumPropagationDuration() const
{
    return segmentPropagationDurationLimit_;
}

Duration Sequence::getMinimumManeuverDuration() const
{
    return minimumManeuverDuration_;
}

Duration Sequence::getMinimumManeuverSeparation() const
{
    return minimumManeuverSeparation_;
}

void Sequence::setMaximumManeuverDuration(const Duration& aMaximumManeuverDuration)
{
    if (!aMaximumManeuverDuration.isDefined())
    {
        maximumManeuverDuration_ = aMaximumManeuverDuration;
        return;
    }

    if (!aMaximumManeuverDuration.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Maximum maneuver duration must be strictly positive.");
    }

    if (minimumManeuverDuration_.isDefined() && minimumManeuverDuration_ > aMaximumManeuverDuration)
    {
        throw ostk::core::error::runtime::Wrong(
            "Maximum maneuver duration must be greater than or equal to the minimum maneuver duration."
        );
    }

    maximumManeuverDuration_ = aMaximumManeuverDuration;
}

void Sequence::setMinimumManeuverDuration(const Duration& aMinimumManeuverDuration)
{
    if (!aMinimumManeuverDuration.isDefined())
    {
        minimumManeuverDuration_ = aMinimumManeuverDuration;
        return;
    }

    if (!aMinimumManeuverDuration.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Minimum maneuver duration must be strictly positive.");
    }

    if (maximumManeuverDuration_.isDefined() && maximumManeuverDuration_ < aMinimumManeuverDuration)
    {
        throw ostk::core::error::runtime::Wrong(
            "Minimum maneuver duration must be less than or equal to the maximum maneuver duration."
        );
    }

    minimumManeuverDuration_ = aMinimumManeuverDuration;
}

void Sequence::setMinimumManeuverSeparation(const Duration& aMinimumManeuverSeparation)
{
    if (!aMinimumManeuverSeparation.isDefined())
    {
        throw ostk::core::error::runtime::Wrong("Minimum maneuver separation must be defined.");
    }

    if (!aMinimumManeuverSeparation.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Minimum maneuver separation must be strictly positive.");
    }

    minimumManeuverSeparation_ = aMinimumManeuverSeparation;
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
        throw ostk::core::error::runtime::Wrong("Repetition count", String::Format("{}", aRepetitionCount));
    }

    Array<Segment::Solution> segmentSolutions = Array<Segment::Solution>::Empty();

    State initialState = aState;
    State finalState = State::Undefined();
    Interval lastManeuverInterval = Interval::Undefined();

    for (Size i = 0; i < aRepetitionCount; ++i)
    {
        for (const Segment& segment : segments_)
        {
            segment.accessEventCondition()->updateTarget(initialState);

            BOOST_LOG_TRIVIAL(debug) << "Solving Segment:\n" << segment << std::endl;

            auto [segmentSolution, newLastManeuverInterval] =
                solveSegment_(initialState, segment, segmentPropagationDurationLimit_, lastManeuverInterval);

            lastManeuverInterval = newLastManeuverInterval;

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
    Array<Segment::Solution> segmentSolutions = Array<Segment::Solution>::Empty();

    State initialState = aState;
    State finalState = State::Undefined();
    Interval lastManeuverInterval = Interval::Undefined();
    bool eventConditionIsSatisfied = false;
    Duration propagationDuration = Duration::Zero();
    const Unique<EventCondition> sequenceCondition(anEventCondition.clone());
    sequenceCondition->updateTarget(initialState);

    while (!eventConditionIsSatisfied && propagationDuration <= aMaximumPropagationDuration)
    {
        for (const Segment& segment : segments_)
        {
            segment.accessEventCondition()->updateTarget(initialState);

            BOOST_LOG_TRIVIAL(debug) << "Solving Segment:\n" << segment << std::endl;

            const Duration segmentPropagationDurationLimit =
                std::min(segmentPropagationDurationLimit_, aMaximumPropagationDuration - propagationDuration);

            auto [segmentSolution, newLastManeuverInterval] =
                solveSegment_(initialState, segment, segmentPropagationDurationLimit, lastManeuverInterval);

            lastManeuverInterval = newLastManeuverInterval;

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

            eventConditionIsSatisfied =
                sequenceCondition->isSatisfied(segmentSolution.states.accessLast(), initialState);

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

Tuple<Segment::Solution, Interval> Sequence::solveSegment_(
    const State& aState,
    const Segment& aSegment,
    const Duration& aMaximumPropagationDuration,
    const Interval& aLastManeuverInterval
) const
{
    // If we're not dealing with a maneuver segment, we can just solve it normally
    if (aSegment.getType() != Segment::Type::Maneuver)
    {
        return {aSegment.solve(aState, aMaximumPropagationDuration), aLastManeuverInterval};
    }

    // If we're dealing with a maneuver segment, we need to solve maneuver by maneuver to account for maneuver-related
    // constraints
    Array<State> states = Array<State>::Empty();
    Array<Interval> maneuverIntervals = Array<Interval>::Empty();
    states.add(aState);
    bool conditionIsSatisfied = false;
    const Instant maximumInstant = aState.accessInstant() + aMaximumPropagationDuration;
    Instant coastToInstant = Instant::Undefined();
    Segment segmentToSolve = aSegment;

    if (aLastManeuverInterval.isDefined() &&
        (aState.accessInstant() - aLastManeuverInterval.getEnd()) < minimumManeuverSeparation_)
    {
        coastToInstant = aLastManeuverInterval.getEnd() + minimumManeuverSeparation_;
        segmentToSolve = buildCoastSegment_(aSegment, "Coast (Minimum Maneuver Separation Constraint)");
    }

    while (true)
    {
        // Coast
        if (segmentToSolve.getType() == Segment::Type::Coast)
        {
            Segment::Solution coastSegmentSolution =
                segmentToSolve.solve(states.accessLast(), coastToInstant - states.accessLast().accessInstant());
            conditionIsSatisfied = coastSegmentSolution.conditionIsSatisfied;

            const Array<State>& segmentStates =
                Array<State>(coastSegmentSolution.states.begin() + 1, coastSegmentSolution.states.end());
            states.add(segmentStates);

            if (conditionIsSatisfied)
            {
                break;
            }

            segmentToSolve = aSegment;
            continue;
        }

        // Maneuver
        Segment::Solution maneuverSegmentSolution =
            segmentToSolve.solveNextManeuver(states.accessLast(), maximumInstant - states.accessLast().accessInstant());
        conditionIsSatisfied = maneuverSegmentSolution.conditionIsSatisfied;

        const Array<Maneuver> maneuverSegmentSolutionManeuvers =
            maneuverSegmentSolution.extractManeuvers(aState.accessFrame());

        // There were no maneuvers to be considered
        if (maneuverSegmentSolutionManeuvers.isEmpty())
        {
            const Array<State>& segmentStates =
                Array<State>(maneuverSegmentSolution.states.begin() + 1, maneuverSegmentSolution.states.end());
            states.add(segmentStates);

            break;
        }

        if (maneuverSegmentSolutionManeuvers.getSize() > 1)
        {
            throw ostk::core::error::RuntimeError(
                "More than one maneuver found when solving maneuver segment with constraints."
            );
        }

        const Maneuver candidateManeuver = maneuverSegmentSolutionManeuvers.accessFirst();

        // Check minimum maneuver duration constraint
        if (minimumManeuverDuration_.isDefined() &&
            candidateManeuver.getInterval().getDuration() < minimumManeuverDuration_)
        {
            BOOST_LOG_TRIVIAL(debug
            ) << "Maneuver duration is less than the minimum maneuver duration. Skipping the maneuver."
              << std::endl;
            segmentToSolve = buildCoastSegment_(aSegment, "Coast (Minimum Maneuver Duration Constraint)");
            coastToInstant = candidateManeuver.getInterval().getEnd();
            continue;
        }

        // Check maximum maneuver duration constraint
        if (maximumManeuverDuration_.isDefined() &&
            candidateManeuver.getInterval().getDuration() > maximumManeuverDuration_)
        {
            BOOST_LOG_TRIVIAL(debug) << "Maneuver duration is greater than the maximum maneuver duration. Shortening "
                                        "the maneuver around its center."
                                     << std::endl;
            const Shared<Thruster> shortenedManeuverThruster = buildThrusterDynamicsWithManeuverIntervals_(
                segmentToSolve,
                {Interval::Centered(
                    candidateManeuver.getInterval().getCenter(), maximumManeuverDuration_, Interval::Type::Closed
                )},
                "(Shortening Maneuver)"
            );
            segmentToSolve = Segment::Maneuver(
                segmentToSolve.getName() + " (Shortened Maneuver)",
                segmentToSolve.getEventCondition(),
                shortenedManeuverThruster,
                buildCoastDynamics_(segmentToSolve),
                segmentToSolve.getNumericalSolver()
            );
            coastToInstant = candidateManeuver.getInterval().getEnd();
            continue;
        }

        const Array<State>& segmentStates =
            Array<State>(maneuverSegmentSolution.states.begin() + 1, maneuverSegmentSolution.states.end());
        states.add(segmentStates);
        maneuverIntervals.add(candidateManeuver.getInterval());

        if (conditionIsSatisfied)
        {
            break;
        }
        BOOST_LOG_TRIVIAL(debug) << "Maneuver accepted, adding minimum maneuver separation coasting." << std::endl;
        segmentToSolve = buildCoastSegment_(aSegment, "Coast (Minimum Maneuver Separation Constraint)");
        coastToInstant = coastToInstant.isDefined()
                           ? std::max(states.accessLast().accessInstant() + minimumManeuverSeparation_, coastToInstant)
                           : states.accessLast().accessInstant() + minimumManeuverSeparation_;
    }

    // Utilize the Heterogeneous Guidance Law to limit the thruster contributions to the actual burn times, otherwise
    // the maneuver extraction will be incorrect (as it will compute contributions all throughout the segment solution
    // interval).
    const Shared<Thruster> newThruster =
        buildThrusterDynamicsWithManeuverIntervals_(aSegment, maneuverIntervals, "(Maneuvering Constraints)");
    Array<Shared<Dynamics>> newDynamics = buildCoastDynamics_(aSegment);
    newDynamics.add(newThruster);

    const Interval lastManeuverInterval =
        maneuverIntervals.isEmpty() ? Interval::Undefined() : maneuverIntervals.accessLast();

    return {
        Segment::Solution(aSegment.getName(), newDynamics, states, conditionIsSatisfied, Segment::Type::Maneuver),
        lastManeuverInterval
    };
}

Array<Shared<Dynamics>> Sequence::buildCoastDynamics_(const Segment& aSegment) const
{
    const Shared<Thruster> thrusterDynamics = aSegment.getThrusterDynamics();

    Array<Shared<Dynamics>> nonThrusterDynamics = Array<Shared<Dynamics>>::Empty();
    nonThrusterDynamics.reserve(aSegment.getDynamics().getSize() - 1);

    for (const Shared<Dynamics>& dynamic : aSegment.getDynamics())
    {
        if (dynamic != thrusterDynamics)
        {
            nonThrusterDynamics.add(dynamic);
        }
    }

    return nonThrusterDynamics;
}

Segment Sequence::buildCoastSegment_(const Segment& aSegment, const String& aSuffix) const
{
    const Array<Shared<Dynamics>> coastDynamics = buildCoastDynamics_(aSegment);

    return Segment::Coast(
        aSegment.getName() + " " + aSuffix, aSegment.getEventCondition(), coastDynamics, aSegment.getNumericalSolver()
    );
}

Shared<Thruster> Sequence::buildThrusterDynamicsWithManeuverIntervals_(
    const Segment& aSegment, const Array<Interval>& aManeuverIntervals, const String& aSuffix
) const
{
    const Shared<Thruster> thrusterDynamics = aSegment.getThrusterDynamics();
    const Shared<const GuidanceLaw> originalGuidanceLaw = thrusterDynamics->getGuidanceLaw();
    const Shared<HeterogeneousGuidanceLaw> heterogeneousGuidanceLaw = std::make_shared<HeterogeneousGuidanceLaw>();

    for (const Interval& maneuverInterval : aManeuverIntervals)
    {
        heterogeneousGuidanceLaw->addGuidanceLaw(
            std::const_pointer_cast<GuidanceLaw>(originalGuidanceLaw), maneuverInterval
        );
    }

    return std::make_shared<Thruster>(
        thrusterDynamics->getSatelliteSystem(), heterogeneousGuidanceLaw, thrusterDynamics->getName() + " " + aSuffix
    );
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

    ostk::core::utils::Print::Line(anOutputStream)
        << "Minimum Maneuver Duration:"
        << (minimumManeuverDuration_.isDefined() ? minimumManeuverDuration_.toString() : "Undefined");

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
