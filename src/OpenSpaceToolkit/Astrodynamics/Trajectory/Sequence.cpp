/// Apache License 2.0

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <OpenSpaceToolkit/Core/Error/Runtime/ToBeImplemented.hpp>
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
      minimumManeuverSeparation_(Duration::Undefined()),
      maximumManeuverDuration_(Duration::Undefined()),
      maximumManeuverDurationStrategy_(MaximumManeuverDurationViolationStrategy::Fail)
{
    if (!aMaximumPropagationDuration.isDefined())
    {
        throw ostk::core::error::runtime::Wrong("Maximum propagation duration must be defined.");
    }

    if (!aMaximumPropagationDuration.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Maximum propagation duration must be strictly positive.");
    }

    this->setMinimumManeuverDuration(aMinimumManeuverDuration);

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

Sequence::MaximumManeuverDurationViolationStrategy Sequence::getMaximumManeuverDurationStrategy() const
{
    return maximumManeuverDurationStrategy_;
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
    if (aMinimumManeuverSeparation.isDefined() && !aMinimumManeuverSeparation.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Minimum maneuver separation must be strictly positive.");
    }

    minimumManeuverSeparation_ = aMinimumManeuverSeparation;
}

void Sequence::setMaximumManeuverDurationStrategy(
    const MaximumManeuverDurationViolationStrategy& aMaximumManeuverDurationStrategy
)
{
    maximumManeuverDurationStrategy_ = aMaximumManeuverDurationStrategy;
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
                this->solveSegment_(initialState, segment, segmentPropagationDurationLimit_, lastManeuverInterval);

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
    // If we're not dealing with a maneuver-related constraints
    if (aSegment.getType() != Segment::Type::Maneuver ||
        (!maximumManeuverDuration_.isDefined() && !minimumManeuverDuration_.isDefined() &&
         !minimumManeuverSeparation_.isDefined()))
    {
        return {aSegment.solve(aState, aMaximumPropagationDuration), aLastManeuverInterval};
    }

    // If we're dealing with a maneuver segment, we need to solve maneuver by maneuver to account for maneuver-related
    // constraints
    Array<State> unifiedStates = Array<State>::Empty();
    Array<Interval> unifiedManeuverIntervals = Array<Interval>::Empty();
    const Shared<HeterogeneousGuidanceLaw> unifiedGuidanceLaw = std::make_shared<HeterogeneousGuidanceLaw>();

    unifiedStates.add(aState);
    bool subsegmentConditionIsSatisfied = false;
    const Instant maximumInstant = aState.accessInstant() + aMaximumPropagationDuration;
    Instant nextInstantToCoastTo = Instant::Undefined();
    Segment nextSubsegmentToSolve = aSegment;

    // Helper lambda to update nextInstantToCoastTo with a new candidate instant
    const auto updateNextCoastInstant = [&maximumInstant, &nextInstantToCoastTo](const Instant& candidateInstant) -> Instant
    {
        const Instant clampedCandidate = std::min(candidateInstant, maximumInstant);
        return nextInstantToCoastTo.isDefined() ? std::max(nextInstantToCoastTo, clampedCandidate) : clampedCandidate;
    };

    // Account for maneuver separation w.r.t. the last maneuver in the sequence
    if (aLastManeuverInterval.isDefined() && minimumManeuverSeparation_.isDefined() &&
        (aState.accessInstant() - aLastManeuverInterval.getEnd()) < minimumManeuverSeparation_)
    {
        nextInstantToCoastTo = updateNextCoastInstant(aLastManeuverInterval.getEnd() + minimumManeuverSeparation_);
        nextSubsegmentToSolve =
            aSegment.toCoastSegment(aSegment.getName() + " (Coast - Minimum Maneuver Separation Constraint)");
    }

    const Duration subsegmentManeuverMargin = Duration::Seconds(10.0);

    while (maximumInstant > unifiedStates.accessLast().accessInstant())
    {
        // Coast
        if (nextSubsegmentToSolve.getType() == Segment::Type::Coast)
        {
            Segment::Solution coastSegmentSolution = nextSubsegmentToSolve.solve(
                unifiedStates.accessLast(),
                std::min(
                    nextInstantToCoastTo - unifiedStates.accessLast().accessInstant(),
                    maximumInstant - unifiedStates.accessLast().accessInstant()
                )
            );
            subsegmentConditionIsSatisfied = coastSegmentSolution.conditionIsSatisfied;
            unifiedStates.add(Array<State>(coastSegmentSolution.states.begin() + 1, coastSegmentSolution.states.end()));

            if (subsegmentConditionIsSatisfied)
            {
                break;
            }

            nextSubsegmentToSolve = aSegment;
            continue;
        }

        Segment::Solution maneuverSubsegmentSolution = nextSubsegmentToSolve.solveToNextManeuver(
            unifiedStates.accessLast(), maximumInstant - unifiedStates.accessLast().accessInstant()
        );
        subsegmentConditionIsSatisfied = maneuverSubsegmentSolution.conditionIsSatisfied;

        const Array<Maneuver> subsegmentManeuvers = maneuverSubsegmentSolution.extractManeuvers(aState.accessFrame());

        // There were no maneuvers to be considered
        if (subsegmentManeuvers.isEmpty())
        {
            unifiedStates.add(
                Array<State>(maneuverSubsegmentSolution.states.begin() + 1, maneuverSubsegmentSolution.states.end())
            );
            break;
        }

        if (subsegmentManeuvers.getSize() > 1)
        {
            throw ostk::core::error::RuntimeError(
                "More than one maneuver found when solving maneuver segment with constraints."
            );
        }

        const Maneuver nextManeuverCandidate = subsegmentManeuvers.accessFirst();

        // Check minimum maneuver duration constraint
        if (minimumManeuverDuration_.isDefined() &&
            nextManeuverCandidate.getInterval().getDuration() < minimumManeuverDuration_)
        {
            BOOST_LOG_TRIVIAL(debug
            ) << "Maneuver duration is less than the minimum maneuver duration. Skipping the maneuver."
              << std::endl;
            nextSubsegmentToSolve = aSegment.toCoastSegment(
                aSegment.getName() + " (Skipping Maneuver due to Minimum Maneuver Duration Constraint)"
            );
            nextInstantToCoastTo = updateNextCoastInstant(nextManeuverCandidate.getInterval().getEnd() + subsegmentManeuverMargin);
            continue;
        }

        // Check maximum maneuver duration constraint
        if (maximumManeuverDuration_.isDefined() &&
            nextManeuverCandidate.getInterval().getDuration() > maximumManeuverDuration_)
        {
            BOOST_LOG_TRIVIAL(debug) << "Maneuver duration is greater than the maximum maneuver duration, handling... "
                                     << std::endl;

            switch (maximumManeuverDurationStrategy_)
            {
                case MaximumManeuverDurationViolationStrategy::Fail:
                {
                    throw ostk::core::error::RuntimeError(
                        "Maneuver duration exceeds maximum maneuver duration constraint, change the maximum maneuver "
                        "duration strategy to prevent the Sequence from failing."
                    );
                }

                case MaximumManeuverDurationViolationStrategy::Skip:
                {
                    nextSubsegmentToSolve = aSegment.toCoastSegment(
                        aSegment.getName() + "(Coast - Skipping Maneuver due to Maximum Maneuver Duration Constraint)"
                    );
                    nextInstantToCoastTo = updateNextCoastInstant(nextManeuverCandidate.getInterval().getEnd() + subsegmentManeuverMargin);
                    break;
                }

                case MaximumManeuverDurationViolationStrategy::Split:
                {
                    const Shared<Thruster> shortenedManeuverThruster = this->buildThrusterDynamicsWithManeuverIntervals_(
                        aSegment,
                        {Interval::Closed(
                            nextManeuverCandidate.getInterval().getStart(),
                            nextManeuverCandidate.getInterval().getStart() + maximumManeuverDuration_
                        )},
                        "(Sliced Maneuver)"
                    );
                    nextSubsegmentToSolve = aSegment.toManeuverSegment(
                        shortenedManeuverThruster, aSegment.getName() + " (Sliced Maneuver)"
                    );
                    nextInstantToCoastTo = updateNextCoastInstant(nextManeuverCandidate.getInterval().getStart() + maximumManeuverDuration_);
                    break;
                }

                case MaximumManeuverDurationViolationStrategy::Center:
                {
                    const Shared<Thruster> shortenedManeuverThruster = this->buildThrusterDynamicsWithManeuverIntervals_(
                        aSegment,
                        {Interval::Centered(
                            nextManeuverCandidate.getInterval().getCenter(),
                            maximumManeuverDuration_,
                            Interval::Type::Closed
                        )},
                        "(Center Maneuver)"
                    );
                    nextSubsegmentToSolve = aSegment.toManeuverSegment(
                        shortenedManeuverThruster, aSegment.getName() + " (Centered Maneuver)"
                    );
                    nextInstantToCoastTo = updateNextCoastInstant(nextManeuverCandidate.getInterval().getEnd() + subsegmentManeuverMargin);
                    break;
                }

                default:
                {
                    throw ostk::core::error::runtime::ToBeImplemented("Maximum maneuver duration strategy");
                }
            }

            continue;
        }

        // Maneuver has been accepted
        BOOST_LOG_TRIVIAL(debug) << "Maneuver accepted." << std::endl;
        unifiedStates.add(
            Array<State>(maneuverSubsegmentSolution.states.begin() + 1, maneuverSubsegmentSolution.states.end())
        );
        unifiedManeuverIntervals.add(nextManeuverCandidate.getInterval());
        unifiedGuidanceLaw->addGuidanceLaw(
            maneuverSubsegmentSolution.getThrusterDynamics()->getGuidanceLaw(),
            nextManeuverCandidate.getInterval()
        );

        if (subsegmentConditionIsSatisfied)
        {
            break;
        }

        // Account for minimum maneuver separation
        BOOST_LOG_TRIVIAL(debug) << "Accounting for minimum maneuver separation." << std::endl;
        nextSubsegmentToSolve =
            aSegment.toCoastSegment(aSegment.getName() + " (Coast - Minimum Maneuver Separation Constraint)");
        if (minimumManeuverSeparation_.isDefined())
        {
            nextInstantToCoastTo = updateNextCoastInstant(unifiedManeuverIntervals.accessLast().getEnd() + minimumManeuverSeparation_);
        }
    }

    Array<Shared<Dynamics>> unifiedDynamics = aSegment.getFreeDynamics();
    unifiedDynamics.add(std::make_shared<Thruster>(
        aSegment.getThrusterDynamics()->getSatelliteSystem(),
        unifiedGuidanceLaw,
        aSegment.getName() + " (Maneuvering Constraints)"
    ));

    return {
        Segment::Solution(
            aSegment.getName(), unifiedDynamics, unifiedStates, subsegmentConditionIsSatisfied, Segment::Type::Maneuver
        ),
        unifiedManeuverIntervals.isEmpty() ? aLastManeuverInterval : unifiedManeuverIntervals.accessLast()
    };
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
        heterogeneousGuidanceLaw->addGuidanceLaw(originalGuidanceLaw, maneuverInterval);
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
