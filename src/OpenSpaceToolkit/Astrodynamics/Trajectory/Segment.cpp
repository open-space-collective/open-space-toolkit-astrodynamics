/// Apache License 2.0

#include <numeric>

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

Array<flightManeuver> Segment::Solution::extractManeuvers(const Shared<const Frame>& aFrameSPtr) const
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

    // Check if there are any breaks in the thrusting (stop and start) and split the dynamics into separate maneuvers
    Array<Pair<Size, Size>> maneuverBlockStartStopIndices = Array<Pair<Size, Size>>::Empty();
    Integer maneuverStart = Integer::Undefined();
    for (Size i = 0; i < numberOfStates; i++)
    {
        if (fullSegmentContributions.row(i).norm() != 0.0)  // If thrusting
        {
            // If a new block hasn't started yet, mark its start
            if (!maneuverStart.isDefined())
            {
                maneuverStart = i;
            }

            // If end of segment is thrusting, close last block
            if (i == numberOfStates - 1)
            {
                // Store stop index as i + 1 because you don't get a chance to "close this
                // block" by seeing the thrust go to zero on the next iteration, since the loop ends on this iteration
                maneuverBlockStartStopIndices.add(Pair<Size, Size>(maneuverStart, i + 1));
            }
        }
        else  // If not thrusting
        {
            // If we have reached the end of a block, save the start and end indices
            if (maneuverStart.isDefined())
            {
                maneuverBlockStartStopIndices.add(Pair<Size, Size>(maneuverStart, i));

                maneuverStart = Integer::Undefined();  // Close the block by marking the start as underfined
            }
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

    Array<flightManeuver> extractedManeuvers = Array<flightManeuver>::Empty();
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

        extractedManeuvers.add(flightManeuver(maneuverStatesBlock));
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
    for (auto aCoordinateSubsetSPtr : aCoordinateSubsetSPtrArray)
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
        definitiveCoordinateSubsetArray = aDynamicsSPtr->getWriteCoordinateSubsets();
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
    const Angle& aMaximumAllowedAngularOffset
)
    : name_(aName),
      type_(aType),
      eventCondition_(anEventConditionSPtr),
      freeDynamicsArray_(aFreeDynamicsArray),
      thrusterDynamicsSPtr_(aThrusterDynamics),
      numericalSolver_(aNumericalSolver),
      constantManeuverDirectionLocalOrbitalFrameFactory_(aLocalOrbitalFrameFactory),
      constantManeuverDirectionMaximumAllowedAngularOffset_(aMaximumAllowedAngularOffset)
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

const Shared<EventCondition>& Segment::accessEventCondition() const
{
    return eventCondition_;
}

const NumericalSolver& Segment::accessNumericalSolver() const
{
    return numericalSolver_;
}

Segment Segment::toCoastSegment(const String& aName) const
{
    return {
        aName.isEmpty() ? name_ : aName,
        Segment::Type::Coast,
        eventCondition_,
        freeDynamicsArray_,
        nullptr,
        numericalSolver_,
        constantManeuverDirectionLocalOrbitalFrameFactory_,
        constantManeuverDirectionMaximumAllowedAngularOffset_,
    };
}

Segment Segment::toManeuverSegment(const Shared<Thruster>& aThrusterDynamics, const String& aName) const
{
    return {
        aName.isEmpty() ? name_ : aName,
        Segment::Type::Maneuver,
        eventCondition_,
        freeDynamicsArray_,
        aThrusterDynamics,
        numericalSolver_,
        constantManeuverDirectionLocalOrbitalFrameFactory_,
        constantManeuverDirectionMaximumAllowedAngularOffset_,
    };
}

Segment::Solution Segment::solve(const State& aState, const Duration& maximumPropagationDuration) const
{
    return solve_(aState, maximumPropagationDuration, true);
}

Segment::Solution Segment::solveToNextManeuver(const State& aState, const Duration& maximumPropagationDuration) const
{
    return solve_(aState, maximumPropagationDuration, false);
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
    const NumericalSolver& aNumericalSolver
)
{
    return {
        aName,
        Segment::Type::Maneuver,
        anEventConditionSPtr,
        aDynamicsArray,
        aThrusterDynamics,
        aNumericalSolver,
    };
}

Segment Segment::ConstantLocalOrbitalFrameDirectionManeuver(
    const String& aName,
    const Shared<EventCondition>& anEventConditionSPtr,
    const Shared<Thruster>& aThrusterDynamics,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver,
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory,
    const Angle& aMaximumAllowedAngularOffset
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
    };
}

Segment::Solution Segment::solve_(
    const State& aState, const Duration& maximumPropagationDuration, const bool& allowMultipleManeuvers
) const
{
    const Segment::Solution solution = this->solveWithDynamics_(
        aState, maximumPropagationDuration, freeDynamicsArray_, thrusterDynamicsSPtr_, allowMultipleManeuvers
    );

    // If we're not forcing a constant maneuver direction in the Local Orbital Frame, return the solution
    if (this->constantManeuverDirectionLocalOrbitalFrameFactory_ == nullptr ||
        !this->constantManeuverDirectionLocalOrbitalFrameFactory_->isDefined())
    {
        return solution;
    }

    const Array<flightManeuver> solutionManeuvers = solution.extractManeuvers(aState.accessFrame());
    if (solutionManeuvers.isEmpty())
    {
        return solution;
    }

    Shared<HeterogeneousGuidanceLaw> heterogeneousGuidanceLaw =
        std::make_shared<HeterogeneousGuidanceLaw>(HeterogeneousGuidanceLaw());

    // TBI: in the future, we might want to solve in-between maneuvers.
    for (const flightManeuver& solutionManeuver : solutionManeuvers)
    {
        const Shared<ConstantThrust> constantThrust = std::make_shared<ConstantThrust>(ConstantThrust::FromManeuver(
            solutionManeuver,
            this->constantManeuverDirectionLocalOrbitalFrameFactory_,
            this->constantManeuverDirectionMaximumAllowedAngularOffset_
        ));
        heterogeneousGuidanceLaw->addGuidanceLaw(constantThrust, solutionManeuver.getInterval());
    }

    const Shared<Thruster> heterogeneousThrustDynamicsSPtr = std::make_shared<Thruster>(
        thrusterDynamicsSPtr_->getSatelliteSystem(),
        heterogeneousGuidanceLaw,
        thrusterDynamicsSPtr_->getName() + " (With Heterogeneous Guidance Law)"
    );

    return this->solveWithDynamics_(
        aState, maximumPropagationDuration, freeDynamicsArray_, heterogeneousThrustDynamicsSPtr, allowMultipleManeuvers
    );
}

Segment::Solution Segment::solveWithDynamics_(
    const State& aState,
    const Duration& maximumPropagationDuration,
    const Array<Shared<Dynamics>>& aFreeDynamicsArray,
    const Shared<Thruster>& aThrusterDynamics,
    const bool& allowMultipleManeuvers
) const
{
    // Combine free dynamics and thruster dynamics into a single array
    Array<Shared<Dynamics>> aDynamicsArray = aFreeDynamicsArray;
    if (aThrusterDynamics != nullptr)
    {
        aDynamicsArray.add(aThrusterDynamics);
    }

    // Determine the event condition to use for solving
    Shared<EventCondition> eventConditionToUse = eventCondition_;
    bool needsConditionReevaluation = false;

    if (type_ != Segment::Type::Coast && !allowMultipleManeuvers)
    {
        const Shared<const GuidanceLaw> guidanceLaw = aThrusterDynamics->getGuidanceLaw();
        const std::function<Real(const State&)> thrustAccelerationNormEvaluator = [guidanceLaw](const State& state
                                                                                  ) -> Real
        {
            const Vector3d positionCoordinates = state.getPosition().inMeters().accessCoordinates();
            const Vector3d velocityCoordinates =
                state.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

            // Set the thrust acceleration to 1.0, as we're only interested to see if it's on or off.
            const Vector3d thrustAcceleration = guidanceLaw->calculateThrustAccelerationAt(
                state.accessInstant(), positionCoordinates, velocityCoordinates, 1.0, state.accessFrame()
            );

            return thrustAcceleration.norm();
        };

        // Use a threshold of 0.5 to determine if the thrust is off, as the thrust acceleration norm will either be 1.0
        // if on, or 0.0 if off.
        const Shared<RealCondition> thrustOffCondition = std::make_shared<RealCondition>(
            "Thrust Off Condition", RealCondition::Criterion::NegativeCrossing, thrustAccelerationNormEvaluator, 0.5
        );

        eventConditionToUse = std::make_shared<LogicalCondition>(
            "Combined Event or Thrust Off Condition",
            LogicalCondition::Type::Or,
            Array<Shared<EventCondition>> {eventCondition_, thrustOffCondition}
        );

        needsConditionReevaluation = true;
    }

    const Propagator propagator = {
        numericalSolver_,
        aDynamicsArray,
    };

    const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
        aState, aState.accessInstant() + maximumPropagationDuration, *eventConditionToUse
    );

    // Expand states based on input state
    const StateBuilder stateBuilder = {aState};

    Array<State> states = Array<State>::Empty();
    states.reserve(propagator.accessNumericalSolver().accessObservedStates().getSize());

    for (const State& state : propagator.accessNumericalSolver().accessObservedStates())
    {
        states.add(stateBuilder.expand(state.inFrame(aState.accessFrame()), aState));
    }

    if (!needsConditionReevaluation)
    {
        return {
            name_,
            aDynamicsArray,
            states,
            conditionSolution.conditionIsSatisfied,
            type_,
        };
    }

    // As the event condition could have terminated due to the thruster off condition, we want to re-evaluate the
    // segment event condition to see if it's satisfied.
    // To do so, we can check the last state against the initial state to see if the event condition is satisfied.
    return {
        name_,
        aDynamicsArray,
        states,
        eventCondition_->isSatisfied(states.accessLast(), aState),
        type_,
    };
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
