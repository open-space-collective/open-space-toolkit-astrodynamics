/// Apache License 2.0

#include <numeric>
#include <optional>

#include <OpenSpaceToolkit/Core/Error/Runtime/Wrong.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::mathematics::object::Vector3d;

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::coordinate::Velocity;

using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::eventcondition::LogicalCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::guidancelaw::HeterogeneousGuidanceLaw;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;
using FlightManeuver = ostk::astrodynamics::flight::Maneuver;

Segment::ManeuverConstraints::ManeuverConstraints(
    const Duration& aMinimumDuration,
    const Duration& aMaximumDuration,
    const Duration& aMinimumSeparation,
    const MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy
)
    : minimumDuration(aMinimumDuration),
      maximumDuration(aMaximumDuration),
      minimumSeparation(aMinimumSeparation),
      maximumDurationStrategy(aMaximumDurationStrategy)
{
    if (minimumDuration.isDefined() && minimumDuration <= Duration::Zero())
    {
        throw ostk::core::error::RuntimeError("Minimum duration must be greater than zero.");
    }

    if (maximumDuration.isDefined() && maximumDuration <= Duration::Zero())
    {
        throw ostk::core::error::RuntimeError("Maximum duration must be greater than zero.");
    }

    if (minimumSeparation.isDefined() && minimumSeparation <= Duration::Zero())
    {
        throw ostk::core::error::RuntimeError("Minimum separation must be greater than zero.");
    }

    if (maximumDuration.isDefined() && minimumDuration.isDefined() && maximumDuration < minimumDuration)
    {
        throw ostk::core::error::RuntimeError("Maximum duration must be greater than minimum duration.");
    }

    if (maximumDuration.isDefined() && !minimumSeparation.isDefined())
    {
        throw ostk::core::error::RuntimeError(
            "Minimum separation of at least 1 second must be defined if maximum duration is defined. This is to "
            "prevent "
            "aliasing issues which can cause sequential maneuver intervals to overlap by a nanosecond."
        );
    }
}

bool Segment::ManeuverConstraints::isDefined() const
{
    return minimumDuration.isDefined() || maximumDuration.isDefined() || minimumSeparation.isDefined();
}

bool Segment::ManeuverConstraints::intervalHasValidMinimumDuration(const Interval& aManeuverInterval) const
{
    if (!minimumDuration.isDefined())
    {
        return true;
    }

    return aManeuverInterval.getDuration() >= minimumDuration;
}

bool Segment::ManeuverConstraints::intervalHasValidMaximumDuration(const Interval& aManeuverInterval) const
{
    if (!maximumDuration.isDefined())
    {
        return true;
    }

    return aManeuverInterval.getDuration() <= maximumDuration;
}

void Segment::ManeuverConstraints::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Maneuver Constraints");
    }

    ostk::core::utils::Print::Line(anOutputStream)
        << "Minimum Duration:" << (this->minimumDuration.isDefined() ? this->minimumDuration.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum Duration:" << (this->maximumDuration.isDefined() ? this->maximumDuration.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Minimum Separation:"
        << (this->minimumSeparation.isDefined() ? this->minimumSeparation.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Maximum Duration Strategy:"
        << Segment::StringFromMaximumManeuverDurationViolationStrategy(this->maximumDurationStrategy);

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const Segment::ManeuverConstraints& aManeuverConstraints)
{
    aManeuverConstraints.print(anOutputStream);
    return anOutputStream;
}

String Segment::StringFromMaximumManeuverDurationViolationStrategy(
    const MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy
)
{
    switch (aMaximumDurationStrategy)
    {
        case MaximumManeuverDurationViolationStrategy::Fail:
            return "Fail";
        case MaximumManeuverDurationViolationStrategy::Skip:
            return "Skip";
        case MaximumManeuverDurationViolationStrategy::TruncateEnd:
            return "TruncateEnd";
        case MaximumManeuverDurationViolationStrategy::TruncateStart:
            return "TruncateStart";
        case MaximumManeuverDurationViolationStrategy::Center:
            return "Center";
        case MaximumManeuverDurationViolationStrategy::Chunk:
            return "Chunk";
        default:
            return "Unknown";
    };
}

Segment::Solution::Solution(
    const String& aName,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Array<State>& aStateArray,
    const bool& aConditionIsSatisfied,
    const Segment::Type& aSegmentType
)
    : name(aName),
      dynamics(aDynamicsArray),
      states(aStateArray),
      conditionIsSatisfied(aConditionIsSatisfied),
      segmentType(aSegmentType)
{
}

Segment::Solution::Solution(
    const String& aName,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Array<State>& aStateArray,
    const bool& aConditionIsSatisfied,
    const Segment::Type& aSegmentType,
    const Array<Interval>& aManeuverIntervals
)
    : name(aName),
      dynamics(aDynamicsArray),
      states(aStateArray),
      conditionIsSatisfied(aConditionIsSatisfied),
      segmentType(aSegmentType),
      maneuverIntervals(aManeuverIntervals)
{
}

const Instant& Segment::Solution::accessStartInstant() const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return this->states.accessFirst().accessInstant();
}

const Instant& Segment::Solution::accessEndInstant() const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return this->states.accessLast().accessInstant();
}

Interval Segment::Solution::getInterval() const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return Interval::Closed(accessStartInstant(), accessEndInstant());
}

Mass Segment::Solution::getInitialMass() const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return Mass::Kilograms(this->states.accessFirst().extractCoordinate(CoordinateSubset::Mass())[0]);
}

Mass Segment::Solution::getFinalMass() const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solution available.");
    }

    return Mass::Kilograms(this->states.accessLast().extractCoordinate(CoordinateSubset::Mass())[0]);
}

Duration Segment::Solution::getPropagationDuration() const
{
    return accessEndInstant() - accessStartInstant();
}

Shared<Thruster> Segment::Solution::getThrusterDynamics() const
{
    if (this->segmentType == Segment::Type::Coast)
    {
        throw ostk::core::error::RuntimeError("Cannot get thruster dynamics from a Coast segment solution.");
    }

    // Find the Thruster dynamics from the segment dynamics
    Shared<Thruster> thrusterDynamics = nullptr;
    for (const Shared<Dynamics>& dynamic : this->dynamics)
    {
        const Shared<Thruster> candidateThruster = std::dynamic_pointer_cast<Thruster>(dynamic);
        if (candidateThruster != nullptr)
        {
            if (thrusterDynamics != nullptr)
            {
                throw ostk::core::error::RuntimeError(
                    "Multiple Thruster dynamics found in Maneuvering segment solution."
                );
            }
            thrusterDynamics = candidateThruster;
        }
    }

    if (thrusterDynamics == nullptr)
    {
        throw ostk::core::error::RuntimeError("No Thruster dynamics found in Maneuvering segment solution.");
    }

    return thrusterDynamics;
}

Real Segment::Solution::computeDeltaV(const Real& aSpecificImpulse) const
{
    // TBM: This is only valid for constant thrust, constant Isp
    if (this->segmentType != Segment::Type::Maneuver)
    {
        return 0.0;
    }

    return aSpecificImpulse * EarthGravitationalModel::gravityConstant *
           std::log(getInitialMass().inKilograms() / getFinalMass().inKilograms());
}

Mass Segment::Solution::computeDeltaMass() const
{
    if (this->segmentType != Segment::Type::Maneuver)
    {
        return Mass::Kilograms(0.0);
    }

    return Mass::Kilograms(getInitialMass().inKilograms() - getFinalMass().inKilograms());
}

Array<FlightManeuver> Segment::Solution::extractManeuvers(const Shared<const Frame>& aFrameSPtr) const
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No states exist within Segment Solution.");
    }

    if (this->segmentType != Segment::Type::Maneuver)
    {
        return {};
    }

    const Shared<Thruster> thrusterDynamics = this->getThrusterDynamics();

    const MatrixXd fullSegmentContributions = this->getDynamicsContribution(
        thrusterDynamics, aFrameSPtr, {CartesianVelocity::Default(), CoordinateSubset::Mass()}
    );

    const Size numberOfStates = static_cast<Size>(fullSegmentContributions.rows());

    Array<Pair<Size, Size>> maneuverBlockStartStopIndices = Array<Pair<Size, Size>>::Empty();

    for (const Interval& maneuverInterval : this->maneuverIntervals)
    {
        Integer blockStart = Integer::Undefined();
        Integer blockEnd = Integer::Undefined();

        for (Size i = 0; i < numberOfStates; i++)
        {
            const Instant& stateInstant = this->states[i].accessInstant();

            if (maneuverInterval.contains(stateInstant))
            {
                if (!blockStart.isDefined())
                {
                    blockStart = i;
                }
                blockEnd = i + 1;  // Exclusive end
            }
        }

        if (blockStart.isDefined() && blockEnd.isDefined())
        {
            maneuverBlockStartStopIndices.add(Pair<Size, Size>(blockStart, blockEnd));
        }
    }

    // If no thrusting has occured during this maneuvering segment (which is possible), return an empty array
    if (maneuverBlockStartStopIndices.isEmpty())
    {
        return {};
    }

    const StateBuilder stateBuilder = {
        aFrameSPtr,
        {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CartesianAcceleration::ThrustAcceleration(),
            CoordinateSubset::MassFlowRate(),
        }
    };

    Array<FlightManeuver> extractedManeuvers = Array<FlightManeuver>::Empty();
    for (const Pair<Size, Size>& startStopPair : maneuverBlockStartStopIndices)
    {
        const Size blockLength = startStopPair.second - startStopPair.first;

        const MatrixXd maneuverContributionBlock =
            fullSegmentContributions.block(startStopPair.first, 0, blockLength, fullSegmentContributions.cols());

        Array<State> maneuverStatesBlock = Array<State>::Empty();
        maneuverStatesBlock.reserve(blockLength);

        for (Size i = 0; i < blockLength; ++i)
        {
            const State& state = this->states[startStopPair.first + i].inFrame(aFrameSPtr);

            VectorXd coordinates(10);
            coordinates.segment<6>(0) = state.extractCoordinates({
                CartesianPosition::Default(),
                CartesianVelocity::Default(),
            });
            coordinates.segment<3>(6) = maneuverContributionBlock.block<1, 3>(i, 0);
            coordinates(9) = fullSegmentContributions(startStopPair.first + i, 3);

            maneuverStatesBlock.add(stateBuilder.build(state.accessInstant(), coordinates));
        }

        extractedManeuvers.add(FlightManeuver(maneuverStatesBlock));
    }

    return extractedManeuvers;
}

Array<State> Segment::Solution::calculateStatesAt(
    const Array<Instant>& anInstantArray, const NumericalSolver& aNumericalSolver
) const
{
    if (this->states.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No states exist within segment.");
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

    for (const Instant& instant : anInstantArray)
    {
        if (instant < this->accessStartInstant() || instant > this->accessEndInstant())
        {
            throw ostk::core::error::RuntimeError("Instant outside of segment.");
        }
    }

    const Propagated propagated = {
        {
            aNumericalSolver,
            this->dynamics,
        },
        this->states,
    };

    return propagated.calculateStatesAt(anInstantArray);
}

MatrixXd Segment::Solution::getDynamicsContribution(
    const Shared<Dynamics>& aDynamicsSPtr,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetSPtrArray
) const
{
    // Check dynamics is part of the segment dynamics
    if (!this->dynamics.contains(aDynamicsSPtr))
    {
        throw ostk::core::error::RuntimeError("Provided dynamics is not part of the segment dynamics.");
    }

    // Extract write coordinate subsets from dynamics
    const Array<Shared<const CoordinateSubset>> dynamicsWriteCoordinateSubsets =
        aDynamicsSPtr->getWriteCoordinateSubsets();

    // Check that the provided coordinate subsets are part of the dynamics write coordinate subsets
    for (const auto& aCoordinateSubsetSPtr : aCoordinateSubsetSPtrArray)
    {
        if (!dynamicsWriteCoordinateSubsets.contains(aCoordinateSubsetSPtr))
        {
            throw ostk::core::error::RuntimeError(String::Format(
                "Provided coordinate subset [{}] is not part of the dynamics write coordinate subsets.",
                aCoordinateSubsetSPtr->getName()
            ));
        }
    }

    // Initialize the definitive coordinate subset array
    Array<Shared<const CoordinateSubset>> definitiveCoordinateSubsetArray = aCoordinateSubsetSPtrArray;

    // Check value for aCoordinateSubsetSPtrArray
    if (aCoordinateSubsetSPtrArray.isEmpty())
    {
        definitiveCoordinateSubsetArray = dynamicsWriteCoordinateSubsets;
    }

    // Extract states size
    const Size numberOfstates = this->states.getSize();

    // Extract dynamics context and behavior relative to state
    Array<Shared<const CoordinateSubset>> dynamicsReadCoordinateSubsets = aDynamicsSPtr->getReadCoordinateSubsets();

    // Construct state builder
    const StateBuilder builder = StateBuilder(aFrameSPtr, dynamicsReadCoordinateSubsets);

    // Compute the size of dynamicsContributionMatrix
    Size dynamicsWriteSize = std::accumulate(
        definitiveCoordinateSubsetArray.begin(),
        definitiveCoordinateSubsetArray.end(),
        0,
        [](int sum, const Shared<const CoordinateSubset>& subset)
        {
            return sum + subset->getSize();
        }
    );

    // Initialize the dynamicsContributionMatrix
    MatrixXd dynamicsContributionMatrix = MatrixXd::Zero(numberOfstates, dynamicsWriteSize);

    // Construct the dynamicsContributionMatrix, state by state (a.k.a row by row)
    for (Index stateIndex = 0; stateIndex < numberOfstates; ++stateIndex)
    {
        const State& state = this->states[stateIndex];

        VectorXd dynamicsContributionAtState = aDynamicsSPtr->computeContribution(
            state.getInstant(), builder.reduce(state.inFrame(aFrameSPtr)).getCoordinates(), aFrameSPtr
        );

        dynamicsContributionMatrix.row(stateIndex) = dynamicsContributionAtState;
    }

    return dynamicsContributionMatrix;
}

MatrixXd Segment::Solution::getDynamicsAccelerationContribution(
    const Shared<Dynamics>& aDynamicsSPtr, const Shared<const Frame>& aFrameSPtr
) const
{
    return this->getDynamicsContribution(aDynamicsSPtr, aFrameSPtr, {CartesianVelocity::Default()});
}

Map<Shared<Dynamics>, MatrixXd> Segment::Solution::getAllDynamicsContributions(const Shared<const Frame>& aFrameSPtr
) const
{
    // Each MatrixXd contains the contribution of a single dynamics for all the segment states
    Map<Shared<Dynamics>, MatrixXd> dynamicsContributionsMap = Map<Shared<Dynamics>, MatrixXd>();

    for (const Shared<Dynamics>& aDynamicsSPtr : this->dynamics)
    {
        MatrixXd dynamicsContribution = this->getDynamicsContribution(aDynamicsSPtr, aFrameSPtr);
        dynamicsContributionsMap.emplace(aDynamicsSPtr, dynamicsContribution);
    }

    return dynamicsContributionsMap;
}

void Segment::Solution::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Segment Solution");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << this->name;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Condition satisfied:" << (this->conditionIsSatisfied ? "True" : "False");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Segment type:" << (this->segmentType == Segment::Type::Coast ? "Coast" : "Maneuver");

    ostk::core::utils::Print::Line(anOutputStream) << "Start instant:" << accessStartInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "End instant:" << accessEndInstant().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Propagation duration:" << (accessEndInstant() - accessStartInstant()).toString();

    if (this->segmentType == Segment::Type::Maneuver)
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Initial mass:" << getInitialMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Final mass:" << getFinalMass().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Total mass consumed:" << computeDeltaMass().toString();
        for (const auto& maneuverInterval : this->maneuverIntervals)
        {
            ostk::core::utils::Print::Line(anOutputStream) << "Maneuver interval:" << maneuverInterval.toString();
        }
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
    const Array<Shared<Dynamics>>& aFreeDynamicsArray,
    const Shared<Thruster>& aThrusterDynamics,
    const NumericalSolver& aNumericalSolver,
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory,
    const Angle& aMaximumAllowedAngularOffset,
    const ManeuverConstraints& aManeuverConstraints
)
    : name_(aName),
      type_(aType),
      eventCondition_(anEventConditionSPtr),
      freeDynamicsArray_(aFreeDynamicsArray),
      thrusterDynamicsSPtr_(aThrusterDynamics),
      numericalSolver_(aNumericalSolver),
      constantManeuverDirectionLocalOrbitalFrameFactory_(aLocalOrbitalFrameFactory),
      constantManeuverDirectionMaximumAllowedAngularOffset_(aMaximumAllowedAngularOffset),
      maneuverConstraints_(aManeuverConstraints)
{
    if (eventCondition_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Event condition");
    }

    if (freeDynamicsArray_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Coast dynamics");
    }

    if (!numericalSolver_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Numerical solver");
    }

    if (type_ == Segment::Type::Coast && thrusterDynamicsSPtr_ != nullptr)
    {
        throw ostk::core::error::runtime::Wrong("Thruster dynamics", "Coast segment cannot have thruster dynamics.");
    }

    if (type_ == Segment::Type::Maneuver && thrusterDynamicsSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Wrong("Thruster dynamics", "Maneuver segment must have thruster dynamics.");
    }

    if (type_ == Segment::Type::Maneuver && thrusterDynamicsSPtr_ != nullptr && !thrusterDynamicsSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Thruster dynamics");
    }

    // Check that coast dynamics doesn't contain any Thruster dynamics
    for (const Shared<Dynamics>& dynamic : freeDynamicsArray_)
    {
        const Shared<Thruster> candidateThruster = std::dynamic_pointer_cast<Thruster>(dynamic);
        if (candidateThruster != nullptr)
        {
            throw ostk::core::error::runtime::Wrong(
                "Coast dynamics", "Coast dynamics array cannot contain Thruster dynamics."
            );
        }
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
    Array<Shared<Dynamics>> allDynamics = freeDynamicsArray_;
    if (thrusterDynamicsSPtr_ != nullptr)
    {
        allDynamics.add(thrusterDynamicsSPtr_);
    }
    return allDynamics;
}

Array<Shared<Dynamics>> Segment::getFreeDynamics() const
{
    return freeDynamicsArray_;
}

NumericalSolver Segment::getNumericalSolver() const
{
    return accessNumericalSolver();
}

Segment::Type Segment::getType() const
{
    return type_;
}

Shared<Thruster> Segment::getThrusterDynamics() const
{
    if (type_ == Segment::Type::Coast)
    {
        throw ostk::core::error::RuntimeError("Coast segment does not have thruster dynamics.");
    }
    return thrusterDynamicsSPtr_;
}

Segment::ManeuverConstraints Segment::getManeuverConstraints() const
{
    return maneuverConstraints_;
}

const Shared<EventCondition>& Segment::accessEventCondition() const
{
    return eventCondition_;
}

const NumericalSolver& Segment::accessNumericalSolver() const
{
    return numericalSolver_;
}

Segment::Solution Segment::solve(
    const State& aState, const Duration& maximumPropagationDuration, Interval previousManeuverInterval
) const
{
    if (type_ == Segment::Type::Coast)
    {
        return solveCoast_(aState, aState.accessInstant() + maximumPropagationDuration);
    }

    // We must solve maneuver by maneuver to get the exact maneuver start and stop times

    Array<State> segmentStates = {aState};
    Array<Interval> acceptedManeuverIntervals = Array<Interval>::Empty();

    // Helper lambda to build a thruster dynamics that only thrusts within the given interval
    const Shared<Thruster> segmentThrusterDynamics = this->getThrusterDynamics();
    const Shared<const GuidanceLaw> originalGuidanceLaw = segmentThrusterDynamics->getGuidanceLaw();

    const auto buildThrusterDynamicsWithinInterval =
        [&originalGuidanceLaw, &segmentThrusterDynamics](const Interval& aManeuverInterval) -> Shared<Thruster>
    {
        const Shared<HeterogeneousGuidanceLaw> heterogeneousGuidanceLaw = std::make_shared<HeterogeneousGuidanceLaw>();
        heterogeneousGuidanceLaw->addGuidanceLaw(originalGuidanceLaw, aManeuverInterval);

        return std::make_shared<Thruster>(
            segmentThrusterDynamics->getSatelliteSystem(), heterogeneousGuidanceLaw, segmentThrusterDynamics->getName()
        );
    };

    bool segmentConditionIsSatisfied = eventCondition_->isSatisfied(aState, aState);

    // Helper lambda to solve a coast segment and update segmentStates
    const Instant maximumInstant = aState.accessInstant() + maximumPropagationDuration;
    const auto solveAndAcceptCoast = [&](const Instant& endInstant) -> bool
    {
        const State& lastState = segmentStates.accessLast();
        const Segment::Solution coastSegmentSolution = solveCoast_(lastState, std::min(endInstant, maximumInstant));

        segmentStates.add(Array<State>(coastSegmentSolution.states.begin() + 1, coastSegmentSolution.states.end()));

        segmentConditionIsSatisfied = coastSegmentSolution.conditionIsSatisfied;

        return segmentConditionIsSatisfied;
    };

    // Helper lambda to solve a single maneuver and extract results
    const auto solveSingleManeuver = [&](const Shared<Thruster>& thrusterDynamics
                                     ) -> std::pair<Segment::Solution, std::optional<FlightManeuver>>
    {
        const State& lastState = segmentStates.accessLast();
        const Segment::Solution coastSolution = solveUntilThrusterOn_(lastState, maximumInstant, thrusterDynamics);

        if (coastSolution.conditionIsSatisfied)
        {
            return {coastSolution, std::nullopt};
        }

        // Only adding the coast states after the early return above, otherwise, the caller to this
        // function will append them a second time.
        segmentStates.add(Array<State>(coastSolution.states.begin() + 1, coastSolution.states.end()));

        Instant maximumManeuverSolutionInstant = maximumInstant;

        if (maneuverConstraints_.maximumDuration.isDefined() &&
            maneuverConstraints_.maximumDurationStrategy == MaximumManeuverDurationViolationStrategy::Chunk)
        {
            // Performance when considering a maximum maneuver duration constraint for the "Chunk" strategy
            // can be optimized by eliminating redundant solver iterations and unnecessary trimming.
            //
            // Since the "Chunk" strategy does not need to know when the burn is going to end, we can safely
            // assume that the burn (or chunk for that matter) should not take longer than said maximum duration.
            maximumManeuverSolutionInstant = std::min(
                segmentStates.accessLast().accessInstant() + maneuverConstraints_.maximumDuration,
                maximumManeuverSolutionInstant
            );
        }

        const Segment::Solution maneuverSolution =
            solveUntilThrusterOff_(segmentStates.accessLast(), maximumManeuverSolutionInstant, thrusterDynamics);

        if (maneuverSolution.states.getSize() <= 2)
        {
            return {maneuverSolution, std::nullopt};
        }

        const Array<FlightManeuver> maneuvers = maneuverSolution.extractManeuvers(aState.accessFrame());

        if (maneuvers.isEmpty())
        {
            return {maneuverSolution, std::nullopt};
        }

        return {maneuverSolution, maneuvers.accessFirst()};
    };

    const auto constructLOFCompliantManeuverSolution = [&](const Segment::Solution& maneuverSolution,
                                                           const FlightManeuver& maneuver) -> Segment::Solution
    {
        if (this->constantManeuverDirectionLocalOrbitalFrameFactory_ == nullptr ||
            !this->constantManeuverDirectionLocalOrbitalFrameFactory_->isDefined())
        {
            return maneuverSolution;
        }

        return constructLOFCompliantManeuverSolution_(segmentStates.accessLast(), maneuver);
    };

    // This is necessary for scenarios where we have a variable attitude guidance law that produces a maneuver (t0, tf)
    // which is then modified into a constant attitude guidance law maneuver.
    // When the next maneuver is solved for, the variable attitude guidance law might produce a thrust at tf because the
    // position velocity state at tf is now different, because it has been propagated with the constant thrust dynamics.
    // This is mostly required for the case that we have no minimum maneuver separation constraint.
    static const Duration shortManeuverThreshold = Duration::Seconds(5.0);

    // This is used to increase the coast duration after a short maneuver, in the case that we are doing several short
    // maneuvers in a row. This is reset when a maneuver has been accepted.
    Size multiplier = 1;

    // Helper lambda to accept a maneuver solution, updating segmentStates and segmentHeterogenousGuidanceLaw
    // It also updates the last maneuver interval and adds the interval to acceptedManeuverIntervals
    const Shared<HeterogeneousGuidanceLaw> segmentHeterogenousGuidanceLaw =
        std::make_shared<HeterogeneousGuidanceLaw>();
    const auto acceptManeuver = [&](const Segment::Solution& maneuverSolution, const FlightManeuver& maneuver) -> void
    {
        segmentStates.add(Array<State>(maneuverSolution.states.begin() + 1, maneuverSolution.states.end()));

        segmentConditionIsSatisfied = maneuverSolution.conditionIsSatisfied;

        previousManeuverInterval = maneuver.getInterval();

        // Track the accepted maneuver interval for explicit maneuver extraction
        acceptedManeuverIntervals.add(previousManeuverInterval);

        // Reset the multiplier to 1, as we have accepted a maneuver
        multiplier = 1;

        segmentHeterogenousGuidanceLaw->addGuidanceLaw(
            maneuverSolution.getThrusterDynamics()->getGuidanceLaw(), previousManeuverInterval
        );
    };

    // Helper lambda to handle maximum duration violation strategies, returning true if the segment condition is
    // satisfied. It also updates the segment states and the guidance law if the maneuver is accepted.
    const auto handleMaximumDurationViolation = [&](const FlightManeuver& candidateManeuver) -> bool
    {
        const Interval candidateManeuverInterval = candidateManeuver.getInterval();

        // helper lambda to solve and accept a constant-lof maneuver
        const auto solveAndAcceptConstantLofManeuver = [&](const Interval& validManeuverInterval) -> bool
        {
            const Shared<Thruster> slicedThruster = buildThrusterDynamicsWithinInterval(validManeuverInterval);
            const Segment::Solution maneuverSolution =
                solveManeuverForInterval_(segmentStates.accessLast(), slicedThruster, validManeuverInterval);
            const FlightManeuver validManeuver = maneuverSolution.extractManeuvers(aState.accessFrame()).accessFirst();
            const Segment::Solution maneuverLOFCompliantSolution =
                constructLOFCompliantManeuverSolution(maneuverSolution, validManeuver);

            acceptManeuver(maneuverLOFCompliantSolution, validManeuver);

            return maneuverLOFCompliantSolution.conditionIsSatisfied;
        };

        switch (maneuverConstraints_.maximumDurationStrategy)
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
                return solveAndAcceptCoast(candidateManeuverInterval.getEnd());
            }

            case MaximumManeuverDurationViolationStrategy::TruncateEnd:
            {
                const Interval validManeuverInterval = Interval::Closed(
                    candidateManeuverInterval.getStart(),
                    candidateManeuverInterval.getStart() + maneuverConstraints_.maximumDuration
                );

                const bool conditionIsSatisfied = solveAndAcceptConstantLofManeuver(validManeuverInterval);

                if (conditionIsSatisfied)
                {
                    return true;
                }

                return solveAndAcceptCoast(candidateManeuverInterval.getEnd());
            }

            case MaximumManeuverDurationViolationStrategy::TruncateStart:
            {
                const Interval validManeuverInterval = Interval::Closed(
                    candidateManeuverInterval.getEnd() - maneuverConstraints_.maximumDuration,
                    candidateManeuverInterval.getEnd()
                );

                return solveAndAcceptConstantLofManeuver(validManeuverInterval);
            }

            case MaximumManeuverDurationViolationStrategy::Center:
            {
                const Interval validManeuverInterval = Interval::Centered(
                    candidateManeuverInterval.getCenter(), maneuverConstraints_.maximumDuration, Interval::Type::Closed
                );

                const bool conditionIsSatisfied = solveAndAcceptConstantLofManeuver(validManeuverInterval);

                if (conditionIsSatisfied)
                {
                    return true;
                }

                return solveAndAcceptCoast(candidateManeuverInterval.getEnd());
            }

            case MaximumManeuverDurationViolationStrategy::Chunk:
            {
                const Interval validManeuverInterval = Interval::Closed(
                    candidateManeuverInterval.getStart(),
                    candidateManeuverInterval.getStart() + maneuverConstraints_.maximumDuration
                );

                return solveAndAcceptConstantLofManeuver(validManeuverInterval);
            }

            default:
            {
                throw ostk::core::error::runtime::ToBeImplemented("Maximum maneuver duration strategy");
            }
        }
    };

    // Main loop: solve maneuver by maneuver, applying constraints
    while (segmentStates.accessLast().accessInstant() < maximumInstant && !segmentConditionIsSatisfied)
    {
        // Check minimum maneuver separation constraint from previous maneuver
        if (maneuverConstraints_.minimumSeparation.isDefined() && previousManeuverInterval.isDefined())
        {
            const Instant endInstant = previousManeuverInterval.getEnd() + maneuverConstraints_.minimumSeparation;

            // No need to coast if we are already past the minimum separation target
            if (segmentStates.accessLast().accessInstant() < endInstant)
            {
                if (solveAndAcceptCoast(endInstant))
                {
                    break;
                }
            }
        }

        const auto [maneuverSubSegmentSolution, subsegmentManeuver] = solveSingleManeuver(segmentThrusterDynamics);

        // No maneuvers found - add states
        if (!subsegmentManeuver.has_value())
        {
            segmentStates.add(
                Array<State>(maneuverSubSegmentSolution.states.begin() + 1, maneuverSubSegmentSolution.states.end())
            );
            segmentConditionIsSatisfied = maneuverSubSegmentSolution.conditionIsSatisfied;

            continue;
        }

        const Interval candidateManeuverInterval = subsegmentManeuver->getInterval();

        if (candidateManeuverInterval.getDuration() < shortManeuverThreshold)
        {
            segmentConditionIsSatisfied =
                solveAndAcceptCoast(candidateManeuverInterval.getEnd() + shortManeuverThreshold * multiplier);

            // Increment the multiplier for each time we skip a short maneuver
            ++multiplier;

            continue;
        }

        // Check minimum maneuver duration constraint
        if (!maneuverConstraints_.intervalHasValidMinimumDuration(candidateManeuverInterval))
        {
            // The extracted maneuver might be a single-point maneuver. This happens as the maneuver window
            // might have been small enough (see note below) for the propagator to only step inside it once. Producing a
            // single-point acceleration block and thus a zero duration Maneuver.
            //
            // In order for the segment to advance, and not get stuck at the single-point maneuver time,
            // we add a small buffer to the end of the maneuver interval.
            //
            // Note: maneuver windows might end up being very small, especially when using state-dependent guidance laws
            // (e.g. Q-Law) in combination with maneuvering constraints (e.g. minimum maneuver duration). An originally
            // short maneuver is filtered out as it doesn't meet the minimum duration constraint. The satellite will
            // coast until the end of the original manevuer and then check for the next maneuver. The guidance law might
            // still produce a yet smaller maneuver (since the satellite might still be at an optimum orbit location),
            // which again gets filtered out. This process continues until the maneuver is finally skipped.
            Instant instantToCoastTo = candidateManeuverInterval.getEnd();

            if (candidateManeuverInterval.getDuration().isZero())
            {
                instantToCoastTo += Duration::Seconds(1.0);
            }

            segmentConditionIsSatisfied = solveAndAcceptCoast(instantToCoastTo);
        }

        // Check maximum maneuver duration constraint
        else if (!maneuverConstraints_.intervalHasValidMaximumDuration(candidateManeuverInterval))
        {
            segmentConditionIsSatisfied = handleMaximumDurationViolation(subsegmentManeuver.value());
        }

        else
        {
            // Candidate maneuver passed all constraints - accept it
            const Segment::Solution maneuverLOFCompliantSolution =
                constructLOFCompliantManeuverSolution(maneuverSubSegmentSolution, subsegmentManeuver.value());
            acceptManeuver(maneuverLOFCompliantSolution, subsegmentManeuver.value());
        }
    }

    // Build final solution
    Array<Shared<Dynamics>> segmentDynamics = freeDynamicsArray_;

    segmentDynamics.add(std::make_shared<Thruster>(
        this->getThrusterDynamics()->getSatelliteSystem(),
        segmentHeterogenousGuidanceLaw,
        this->getThrusterDynamics()->getName() + " (Maneuvering Constraints)"
    ));

    return Segment::Solution(
        name_,
        segmentDynamics,
        segmentStates,
        segmentConditionIsSatisfied,
        Segment::Type::Maneuver,
        acceptedManeuverIntervals
    );
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
    for (const auto& dynamics : freeDynamicsArray_)
    {
        dynamics->print(anOutputStream, false);
    }
    if (thrusterDynamicsSPtr_ != nullptr)
    {
        thrusterDynamicsSPtr_->print(anOutputStream, false);
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
        nullptr,
        aNumericalSolver,
    };
}

Segment Segment::Maneuver(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Shared<Thruster>& aThrusterDynamics,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver,
    const ManeuverConstraints& aManeuverConstraints
)
{
    return {
        aName,
        Segment::Type::Maneuver,
        anEventConditionSPtr,
        aDynamicsArray,
        aThrusterDynamics,
        aNumericalSolver,
        nullptr,
        Angle::Undefined(),
        aManeuverConstraints,
    };
}

Segment Segment::ConstantLocalOrbitalFrameDirectionManeuver(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Shared<Thruster>& aThrusterDynamics,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver,
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory,
    const Angle& aMaximumAllowedAngularOffset,
    const ManeuverConstraints& aManeuverConstraints
)
{
    return {
        aName,
        Segment::Type::Maneuver,
        anEventConditionSPtr,
        aDynamicsArray,
        aThrusterDynamics,
        aNumericalSolver,
        aLocalOrbitalFrameFactory,
        aMaximumAllowedAngularOffset,
        aManeuverConstraints
    };
}

Segment::Solution Segment::solveWithDynamics_(
    const State& aState,
    const Instant& anEndInstant,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Shared<EventCondition>& anEventCondition
) const
{
    const Propagator propagator = {
        numericalSolver_,
        aDynamicsArray,
    };

    const NumericalSolver::ConditionSolution conditionSolution =
        propagator.calculateStateToCondition(aState, anEndInstant, *anEventCondition);

    // Expand states based on input state
    const StateBuilder stateBuilder = {aState};

    Array<State> states = Array<State>::Empty();
    states.reserve(propagator.accessNumericalSolver().accessObservedStates().getSize());

    for (const State& state : propagator.accessNumericalSolver().accessObservedStates())
    {
        states.add(stateBuilder.expand(state.inFrame(aState.accessFrame()), aState));
    }

    return {
        name_,
        aDynamicsArray,
        states,
        conditionSolution.conditionIsSatisfied,
        type_,
    };
}

Array<State> Segment::propagateWithDynamics_(
    const State& aState, const Instant& anEndInstant, const Array<Shared<Dynamics>>& aDynamicsArray
) const
{
    const Propagator propagator = {
        numericalSolver_,
        aDynamicsArray,
    };

    propagator.calculateStateAt(aState, anEndInstant);

    // Expand states based on input state
    const StateBuilder stateBuilder = {aState};

    Array<State> states = Array<State>::Empty();
    states.reserve(propagator.accessNumericalSolver().accessObservedStates().getSize());

    for (const State& state : propagator.accessNumericalSolver().accessObservedStates())
    {
        states.add(stateBuilder.expand(state.inFrame(aState.accessFrame()), aState));
    }

    return states;
}

Segment::Solution Segment::constructLOFCompliantManeuverSolution_(const State& aState, const FlightManeuver& aManeuver)
    const
{
    const Shared<ConstantThrust> constantThrust = std::make_shared<ConstantThrust>(ConstantThrust::FromManeuver(
        aManeuver,
        this->constantManeuverDirectionLocalOrbitalFrameFactory_,
        this->constantManeuverDirectionMaximumAllowedAngularOffset_
    ));

    const Shared<Thruster> constantThrustDynamics = std::make_shared<Thruster>(
        thrusterDynamicsSPtr_->getSatelliteSystem(),
        constantThrust,
        thrusterDynamicsSPtr_->getName() + " (Constant Thrust)"
    );

    return solveManeuverForInterval_(aState, constantThrustDynamics, aManeuver.getInterval());
}

Segment::Solution Segment::solveCoast_(const State& aState, const Instant& anEndInstant) const
{
    Segment::Solution solution = solveWithDynamics_(aState, anEndInstant, freeDynamicsArray_, eventCondition_);
    solution.segmentType = Segment::Type::Coast;

    return solution;
}

Shared<RealCondition> Segment::getThrusterToggleCondition_(const Shared<Thruster>& thrusterDynamics, const bool& isOn)
    const
{
    const Shared<const GuidanceLaw> guidanceLaw = thrusterDynamics->getGuidanceLaw();

    const std::function<Real(const State&)> thrustAccelerationNormEvaluator = [guidanceLaw](const State& state) -> Real
    {
        const Vector3d positionCoordinates = state.getPosition().inMeters().accessCoordinates();
        const Vector3d velocityCoordinates =
            state.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

        const Vector3d thrustAcceleration = guidanceLaw->calculateThrustAccelerationAt(
            state.accessInstant(), positionCoordinates, velocityCoordinates, 1.0, state.accessFrame()
        );

        return thrustAcceleration.norm();
    };

    // Use a threshold of 0.5 to determine if the thrust is off, as the thrust acceleration norm will either be 1.0
    // if on, or 0.0 if off.
    return std::make_shared<RealCondition>(
        "Thrust Toggle Condition",
        isOn ? RealCondition::Criterion::StrictlyPositive : RealCondition::Criterion::StrictlyNegative,
        thrustAccelerationNormEvaluator,
        0.5
    );
}

Segment::Solution Segment::solveUntilThrusterOff_(
    const State& aState, const Instant& anEndInstant, const Shared<Thruster>& thrusterDynamics
) const
{
    const Shared<RealCondition> thrusterToggleCondition = getThrusterToggleCondition_(thrusterDynamics, false);

    // Combine free dynamics and thruster dynamics into a single array
    Array<Shared<Dynamics>> dynamicsArray = freeDynamicsArray_;
    dynamicsArray.add(thrusterDynamics);

    // Solve the segment until the thrust is off
    const Shared<EventCondition> combinedCondition = std::make_shared<LogicalCondition>(
        "Combined Event or Thrust Off Condition",
        LogicalCondition::Type::Or,
        Array<Shared<EventCondition>> {eventCondition_, thrusterToggleCondition}
    );

    Segment::Solution solution = solveWithDynamics_(aState, anEndInstant, dynamicsArray, combinedCondition);

    // As the event condition could have terminated due to the thruster off condition, we want to re-evaluate the
    // segment event condition to see if it's satisfied.
    solution.conditionIsSatisfied = reEvaluateEventCondition_(aState, solution.states);
    solution.segmentType = Segment::Type::Maneuver;
    solution.maneuverIntervals = {Interval::Closed(aState.getInstant(), solution.states.accessLast().getInstant())};

    return solution;
}

Segment::Solution Segment::solveUntilThrusterOn_(
    const State& aState, const Instant& anEndInstant, const Shared<Thruster>& thrusterDynamics
) const
{
    const Shared<RealCondition> thrusterToggleCondition = getThrusterToggleCondition_(thrusterDynamics, true);

    // Solve the segment until the thrust is on
    const Shared<EventCondition> combinedCondition = std::make_shared<LogicalCondition>(
        "Combined Event or Thrust On Condition",
        LogicalCondition::Type::Or,
        Array<Shared<EventCondition>> {eventCondition_, thrusterToggleCondition}
    );

    Segment::Solution solution = solveWithDynamics_(aState, anEndInstant, freeDynamicsArray_, combinedCondition);

    // As the event condition could have terminated due to the thruster on condition, we want to re-evaluate the
    // segment event condition to see if it's satisfied.
    solution.conditionIsSatisfied = reEvaluateEventCondition_(aState, solution.states);
    solution.segmentType = Segment::Type::Coast;

    return solution;
}

Segment::Solution Segment::solveManeuverForInterval_(
    const State& aState, const Shared<Thruster>& thrusterDynamics, const Interval& validManeuverInterval
) const
{
    Array<State> states = Array<State>::Empty();

    // Coast until the start of the maneuver to ensure we begin solving the maneuver at the exact start instant
    const Array<State> coastStates =
        propagateWithDynamics_(aState, validManeuverInterval.getStart(), freeDynamicsArray_);
    states.add(std::move(coastStates));

    const Array<Shared<Dynamics>> dynamicsArray = freeDynamicsArray_ + Array<Shared<Dynamics>> {thrusterDynamics};

    // Solve the maneuver for just the defined interval
    const State lastState = coastStates.isEmpty() ? aState : coastStates.accessLast();
    const Array<State> maneuverStates =
        propagateWithDynamics_(lastState, validManeuverInterval.getEnd(), dynamicsArray);

    // Skip the first maneuver state if we have coast states (it duplicates the last coast state)
    const auto maneuverStartIter = coastStates.isEmpty() ? maneuverStates.begin() : maneuverStates.begin() + 1;
    states.add(Array<State>(maneuverStartIter, maneuverStates.end()));

    return {
        name_,
        dynamicsArray,
        states,
        reEvaluateEventCondition_(aState, states),
        Segment::Type::Maneuver,
        Array<Interval> {validManeuverInterval}
    };
}

bool Segment::reEvaluateEventCondition_(const State& aState, const Array<State>& aStateArray) const
{
    if (aStateArray.getSize() > 1)
    {
        const State& lastState = aStateArray.accessLast();
        const State& secondToLastState = aStateArray[aStateArray.getSize() - 2];
        return eventCondition_->isSatisfied(lastState, secondToLastState);
    }

    if (aStateArray.getSize() == 1)
    {
        const State& lastState = aStateArray.accessLast();
        return eventCondition_->isSatisfied(lastState, aState);
    }

    return eventCondition_->isSatisfied(aState, aState);
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
