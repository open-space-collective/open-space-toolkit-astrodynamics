/// Apache License 2.0
#include <numeric>

#include <OpenSpaceToolkit/Astrodynamics/MissionSequence.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

MissionSequence::MissionSequence(const Dictionary& aDataTree)
    : dataTree_(aDataTree),
      satelliteSystem_(Parser::CreateSatelliteSystem(dataTree_)),
      initialState_(Parser::CreateInitialState(dataTree_, satelliteSystem_)),
      environment_(Parser::CreateEnvironment(dataTree_)),
      dynamics_(Dynamics::FromEnvironment(environment_)),
      sequence_(Parser::CreateSequence(dataTree_, satelliteSystem_, dynamics_)),
      solvedStates_(Array<State>::Empty()),
      solvedAccelerations_(Array<VectorXd>::Empty())
{
}

MissionSequence::~MissionSequence() {}

void MissionSequence::run()
{
    Sequence::Solution sequenceSolution = this->sequence_.solve(initialState_);
    const Array<Instant> comparisonInstants = Parser::CreateComparisonInstants(
        dataTree_, sequenceSolution.accessStartInstant(), sequenceSolution.accessEndInstant()
    );

    std::cout << "A" << std::endl;

    // Need to recompute the states at the comparison instants because the sequence solution only
    // contains the states at the arbitrary sequence instants
    this->solvedStates_ = sequenceSolution.calculateStatesAt(comparisonInstants, this->sequence_.getNumericalSolver());

    // Re-compute and cache dynamics contributions
    // Array<Segment::Solution> segmentSolutionArray = this->sequence_.getSegmentSolutions();

    // Will need to re-compute dynamics contribution for specific states actually

    const Shared<const Frame> frameSPtr = Frame::GCRF();  // Default for now

    std::cout << "B" << std::endl;

    // Each MatrixXd contains the contribution of a single dynamics for all the segment states
    MatrixXd sumAccelerationContributions;

    for (const Shared<Dynamics>& aDynamicsSPtr : this->dynamics_)
    {

        // Extract write coordinates subsets from dynamics
        const Array<Shared<const CoordinatesSubset>> dynamicsWriteCoordinatesSubsets = aDynamicsSPtr->getWriteCoordinatesSubsets();

        // Skip dynamics that do not write CartesianVelocity (i.e. that does not contribute to accelerations)
        if (dynamicsWriteCoordinatesSubsets.contains(CartesianVelocity::Default()))
        {
            MatrixXd dynamicsContribution = this->getDynamicsAccelerationContribution(this->solvedStates_, aDynamicsSPtr, frameSPtr);
            sumAccelerationContributions += dynamicsContribution;
        }
    }

    std::cout << "C" << std::endl;

    for (Index stateIndex = 0; stateIndex < this->solvedStates_.getSize(); ++stateIndex)
    {
        this->solvedAccelerations_.push_back(sumAccelerationContributions.row(stateIndex));
    }

    std::cout << "D" << std::endl;

    // std::cout << solvedAccelerations_ << std::endl;

    // TBI: Does not handle maneuver for now
}

MatrixXd MissionSequence::getDynamicsContribution(
    const Array<State>& aStateArray,
    const Shared<Dynamics>& aDynamicsSPtr,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetSPtrArray
) const
{
    // Check dynamics is part of the segment dynamics
    if (!this->dynamics_.contains(aDynamicsSPtr))
    {
        throw ostk::core::error::RuntimeError("Provided dynamics is not part of the Mission Sequence.");
    }

    // Extract write coordinates subsets from dynamics
    const Array<Shared<const CoordinatesSubset>> dynamicsWriteCoordinatesSubsets =
        aDynamicsSPtr->getWriteCoordinatesSubsets();

    // Check that the provided coordinates subsets are part of the dynamics write coordinates subsets
    for (auto aCoordinatesSubsetSPtr : aCoordinatesSubsetSPtrArray)
    {
        if (!dynamicsWriteCoordinatesSubsets.contains(aCoordinatesSubsetSPtr))
        {
            throw ostk::core::error::RuntimeError(String::Format(
                "Provided coordinates subset [{}] is not part of the dynamics write coordinates subsets.",
                aCoordinatesSubsetSPtr->getName()
            ));
        }
    }

    // Initialize the definitive coordinate subset array
    Array<Shared<const CoordinatesSubset>> definitiveCoordinateSubsetArray = aCoordinatesSubsetSPtrArray;

    // Check value for aCoordinatesSubsetSPtrArray
    if (aCoordinatesSubsetSPtrArray.isEmpty())
    {
        definitiveCoordinateSubsetArray = aDynamicsSPtr->getWriteCoordinatesSubsets();
    }

    // Extract dynamics context and behavior relative to state
    Array<Shared<const CoordinatesSubset>> dynamicsReadCoordinatesSubsets = aDynamicsSPtr->getReadCoordinatesSubsets();

    // Construct state builder
    const StateBuilder builder = StateBuilder(aFrameSPtr, dynamicsReadCoordinatesSubsets);

    // Extract states size
    const Size numberOfstates = aStateArray.getSize();

    // Compute the size of dynamicsContributionMatrix
    Size dynamicsWriteSize = std::accumulate(
        definitiveCoordinateSubsetArray.begin(),
        definitiveCoordinateSubsetArray.end(),
        0,
        [](int sum, const Shared<const CoordinatesSubset>& subset)
        {
            return sum + subset->getSize();
        }
    );

    // Initialize the dynamicsContributionMatrix

    MatrixXd dynamicsContributionMatrix = MatrixXd::Zero(numberOfstates, dynamicsWriteSize);

    // Construct the dynamicsContributionMatrix, state by state (a.k.a row by row)
    for (Index stateIndex = 0; stateIndex < numberOfstates; ++stateIndex)
    {
        const State& state = aStateArray[stateIndex];

        VectorXd dynamicsContributionAtState = aDynamicsSPtr->computeContribution(
            state.getInstant(), builder.reduce(state.inFrame(aFrameSPtr)).getCoordinates(), aFrameSPtr
        );

        dynamicsContributionMatrix.row(stateIndex) = dynamicsContributionAtState;
    }

    return dynamicsContributionMatrix;
}

MatrixXd MissionSequence::getDynamicsAccelerationContribution(
    const Array<State>& aStateArray, const Shared<Dynamics>& aDynamicsSPtr, const Shared<const Frame>& aFrameSPtr
) const
{
    return this->getDynamicsContribution(aStateArray, aDynamicsSPtr, aFrameSPtr, {CartesianVelocity::Default()});
}

const SatelliteSystem& MissionSequence::accessSatelliteSystem() const
{
    if (!satelliteSystem_.isDefined())
    {
        throw ostk::core::error::RuntimeError("No satellite system defined.");
    }
    return satelliteSystem_;
}

const State& MissionSequence::accessInitialState() const
{
    if (!initialState_.isDefined())
    {
        throw ostk::core::error::RuntimeError("No initial state defined.");
    }
    return initialState_;
}

const Environment& MissionSequence::accessEnvironment() const
{
    if (!environment_.isDefined())
    {
        throw ostk::core::error::RuntimeError("No environment defined.");
    }
    return environment_;
}

const Array<Shared<Dynamics>>& MissionSequence::accessDynamics() const
{
    if (dynamics_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No dynamics defined.");
    }
    return dynamics_;
}

const Sequence& MissionSequence::accessSequence() const
{
    return sequence_;
}

const Array<State>& MissionSequence::accessSolvedStates() const
{
    if (solvedStates_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solved states defined.");
    }
    return solvedStates_;
}

Array<VectorXd> MissionSequence::compareResults(const Table& referenceData, const ToolComparison& aToolComparison) const
{
    const Array<QuantityComparison>& quantityComparisons = aToolComparison.quantityComparisons;

    std::cout << "compareResults Start" << std::endl;

    Array<Array<VectorXd>> referenceOutputs = CrossValidator::IngestOutputQuantities(referenceData, aToolComparison);

    if (solvedStates_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solved states defined.");
    }

    if (referenceOutputs.getSize() != solvedStates_.getSize())
    {
        throw ostk::core::error::RuntimeError("Number of reference outputs does not match number of solved states.");
    }

    if (solvedAccelerations_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No solved accelerations defined.");
    }

    std::cout << "compareResults Checks" << std::endl;

    Array<VectorXd> allDeltasWithTool = Array<VectorXd>::Empty();
    allDeltasWithTool.reserve(solvedStates_.getSize());

    for (Size stateIndex = 0; stateIndex < solvedStates_.getSize(); stateIndex++)
    {
        std::cout << "Inside the for loop" << std::endl;

        VectorXd deltaWithTool = CrossValidator::CompareOutputQuantities(
            solvedStates_[stateIndex], this->solvedAccelerations_[stateIndex], referenceOutputs[stateIndex], quantityComparisons
        );  // TBI: Should adapt to compare accelerations as well, need to be able to add a non solvedState based vector to CompareOutputQuantities -> split functions at CrossValidator level IMO

        allDeltasWithTool.add(deltaWithTool);
    }

    return allDeltasWithTool;
}

}  // namespace validation
}  // namespace astro
}  // namespace ostk
