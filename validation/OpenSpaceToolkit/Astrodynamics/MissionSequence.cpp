/// Apache License 2.0

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
      solvedStates_(Array<State>::Empty())
{
}

MissionSequence::~MissionSequence() {}

void MissionSequence::run()
{
    Sequence::Solution sequenceSolution = sequence_.solve(initialState_);
    const Array<Instant> comparisonInstants = Parser::CreateComparisonInstants(
        dataTree_, sequenceSolution.accessStartInstant(), sequenceSolution.accessEndInstant()
    );

    // Need to recompute the states at the comparison instants because the sequence solution only
    // contains the states at the arbitrary sequence instants
    solvedStates_ = sequenceSolution.calculateStatesAt(comparisonInstants, sequence_.getNumericalSolver());
}

const SatelliteSystem& MissionSequence::accessSatelliteSystem() const
{
    if (satelliteSystem_.isDefined())
    {
        throw ostk::core::error::RuntimeError("No satellite system defined.");
    }
    return satelliteSystem_;
}

const State& MissionSequence::accessInitialState() const
{
    if (initialState_.isDefined())
    {
        throw ostk::core::error::RuntimeError("No initial state defined.");
    }
    return initialState_;
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
