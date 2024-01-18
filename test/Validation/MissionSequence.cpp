/// Apache License 2.0

#include <Validation/MissionSequence.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

MissionSequence::MissionSequence(const Dictionary& aDataTree)
    : dataTree_(aDataTree),
      initialStateAndSatelliteSystem_(Parser::CreateInitialStateAndSatelliteSystem(dataTree_)),
      environment_(Parser::CreateEnvironment(dataTree_)),
      dynamics_(Dynamics::FromEnvironment(environment_)),
      sequence_(Parser::CreateSequence(dataTree_, initialStateAndSatelliteSystem_, dynamics_)),
      solvedStates_(Array<State>::Empty())
{
}

MissionSequence::~MissionSequence() {}

void MissionSequence::run()
{
    Sequence::Solution sequenceSolution = sequence_.solve(initialStateAndSatelliteSystem_.first);
    const Array<Instant> comparisonInstants = Parser::CreateComparisonInstants(
        dataTree_, sequenceSolution.accessStartInstant(), sequenceSolution.accessEndInstant()
    );

    // Need to recompute the states at the comparison instants because the sequence solution only
    // contains the states at the arbitrary sequence instants
    solvedStates_ = sequenceSolution.calculateStatesAt(comparisonInstants, sequence_.getNumericalSolver());
}

const Pair<State, SatelliteSystem>& MissionSequence::accessInitialStateAndSatelliteSystem() const
{
    if (initialStateAndSatelliteSystem_.first.isDefined() || initialStateAndSatelliteSystem_.second.isDefined())
    {
        throw ostk::core::error::RuntimeError("No initial state or satellite system defined.");
    }
    return initialStateAndSatelliteSystem_;
}

const Environment& MissionSequence::accessEnvironment() const
{
    if (environment_.isDefined())
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
        throw ostk::core::error::RuntimeError("No states defined.");
    }
    return solvedStates_;
}

Array<VectorXd> MissionSequence::compareResults(const Table& referenceData, const ToolComparison& aToolComparison) const
{
    const Array<QuantityComparison>& quantityComparisons = aToolComparison.quantityComparisons;

    Array<Array<VectorXd>> referenceOutputs = CrossValidator::IngestOutputQuantities(referenceData, aToolComparison);

    Array<VectorXd> allDeltasWithTool = Array<VectorXd>::Empty();
    allDeltasWithTool.reserve(solvedStates_.getSize());
    for (Size stateIndex = 0; stateIndex < solvedStates_.getSize(); stateIndex++)
    {
        VectorXd deltaWithTool = CrossValidator::CompareOutputQuantities(
            solvedStates_[stateIndex], referenceOutputs[stateIndex], quantityComparisons
        );

        allDeltasWithTool.add(deltaWithTool);
    }

    return allDeltasWithTool;
}

}  // namespace validation
}  // namespace astro
}  // namespace ostk
