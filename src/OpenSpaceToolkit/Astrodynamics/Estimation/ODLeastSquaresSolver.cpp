/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimation/ODLeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimation
{

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::StateBuilder;

ODLeastSquaresSolver::Analysis::Analysis(const State& aDeterminedState, const LeastSquaresSolver::Analysis& anAnalysis)
    : determinedState_(aDeterminedState),
      solverAnalysis_(anAnalysis)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const ODLeastSquaresSolver::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void ODLeastSquaresSolver::Analysis::print(std::ostream& anOutputStream) const
{
    ostk::core::utils::Print::Header(anOutputStream, "Orbit Determination Solver Analysis");

    ostk::core::utils::Print::Separator(anOutputStream, "Determined State");
    determinedState_.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Analysis");
    solverAnalysis_.print(anOutputStream);

    ostk::core::utils::Print::Footer(anOutputStream);
}

const State& ODLeastSquaresSolver::Analysis::getDeterminedState() const
{
    return determinedState_;
}

const LeastSquaresSolver::Analysis& ODLeastSquaresSolver::Analysis::getSolverAnalysis() const
{
    return solverAnalysis_;
}

ODLeastSquaresSolver::ODLeastSquaresSolver(
    const Environment& anEnvironment, const NumericalSolver& aNumericalSolver, const LeastSquaresSolver& aSolver
)
    : environment_(anEnvironment),
      propagator_(Propagator::FromEnvironment(aNumericalSolver, anEnvironment)),
      solver_(aSolver)
{
    if (!environment_.hasCentralCelestialObject())
    {
        throw ostk::core::error::runtime::Undefined("Central celestial object");
    }
}

const Environment& ODLeastSquaresSolver::accessEnvironment() const
{
    return environment_;
}

const Propagator& ODLeastSquaresSolver::accessPropagator() const
{
    return propagator_;
}

const LeastSquaresSolver& ODLeastSquaresSolver::accessSolver() const
{
    return solver_;
}

ODLeastSquaresSolver::Analysis ODLeastSquaresSolver::estimateState(
    const State& anInitialGuessState,
    const Array<State>& aReferenceStateArray,
    const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets,
    const std::unordered_map<CoordinateSubset, Real>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, Real>& aReferenceStateSigmas
) const
{
    // Setup state builders
    const StateBuilder propagationStateBuilder(anInitialGuessState);
    const StateBuilder estimationStateBuilder(
        anInitialGuessState.accessFrame(),
        anEstimationCoordinateSubsets.isEmpty() ? anInitialGuessState.getCoordinateSubsets()
                                                : anEstimationCoordinateSubsets
    );

    // Validate estimation subsets
    for (const auto& subset : estimationStateBuilder.getCoordinateSubsets())
    {
        if (!anInitialGuessState.hasSubset(subset))
        {
            throw ostk::core::error::RuntimeError(
                "Input State must contain at least the coordinate subsets that we want to estimate."
            );
        }
    }

    // Define state generation callback
    auto generateStatesCallback = [&](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        const State propagatorState = propagationStateBuilder.expand(aState, anInitialGuessState);
        return propagator_.calculateStatesAt(propagatorState, anInstantArray);
    };

    // Solve least squares problem
    const LeastSquaresSolver::Analysis analysis = solver_.solve(
        estimationStateBuilder.reduce(anInitialGuessState),
        aReferenceStateArray,
        generateStatesCallback,
        anInitialGuessSigmas,
        aReferenceStateSigmas
    );

    // Expand solution state to full state
    const State determinedState = propagationStateBuilder.expand(analysis.accessSolutionState(), anInitialGuessState);

    return Analysis(determinedState, analysis);
}

Orbit ODLeastSquaresSolver::estimateOrbit(
    const State& anInitialGuessState,
    const Array<State>& aReferenceStateArray,
    const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets,
    const std::unordered_map<CoordinateSubset, Real>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, Real>& aReferenceStateSigmas
) const
{
    const Analysis analysis = estimateState(
        anInitialGuessState,
        aReferenceStateArray,
        anEstimationCoordinateSubsets,
        anInitialGuessSigmas,
        aReferenceStateSigmas
    );

    return Orbit(
        Propagated(this->propagator_, {analysis.getDeterminedState()}),
        this->environment_.accessCentralCelestialObject()
    );
}

}  // namespace estimation
}  // namespace astrodynamics
}  // namespace ostk
