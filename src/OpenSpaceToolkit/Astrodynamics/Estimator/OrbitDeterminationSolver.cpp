/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::StateBuilder;

OrbitDeterminationSolver::Analysis::Analysis(const State& anEstimatedState, const LeastSquaresSolver::Analysis& anAnalysis)
    : estimatedState(anEstimatedState),
      estimatorAnalysis(anAnalysis)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const OrbitDeterminationSolver::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void OrbitDeterminationSolver::Analysis::print(std::ostream& anOutputStream) const
{
    ostk::core::utils::Print::Header(anOutputStream, "Orbit Determination Solver Analysis");

    ostk::core::utils::Print::Separator(anOutputStream, "Estimated State");
    estimatedState.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Analysis");
    estimatorAnalysis.print(anOutputStream);

    ostk::core::utils::Print::Footer(anOutputStream);
}

OrbitDeterminationSolver::OrbitDeterminationSolver(
    const Environment& anEnvironment,
    const Shared<const Estimator>& anEstimator,
    const NumericalSolver& aNumericalSolver,
    const Shared<const Frame>& anEstimationFrameSPtr
)
    : environment_(anEnvironment),
      propagator_(Propagator::FromEnvironment(aNumericalSolver, anEnvironment)),
      estimator_(anEstimator),
      estimationFrameSPtr_(anEstimationFrameSPtr)
{
    if (!environment_.hasCentralCelestialObject())
    {
        throw ostk::core::error::runtime::Undefined("Central celestial object");
    }
}

const Environment& OrbitDeterminationSolver::accessEnvironment() const
{
    return environment_;
}

const Propagator& OrbitDeterminationSolver::accessPropagator() const
{
    return propagator_;
}

const Shared<const Estimator>& OrbitDeterminationSolver::accessSolver() const
{
    return estimator_;
}

const Shared<const Frame>& OrbitDeterminationSolver::accessEstimationFrame() const
{
    return estimationFrameSPtr_;
}

OrbitDeterminationSolver::Analysis OrbitDeterminationSolver::estimate(
    const State& anInitialGuessState,
    const Array<State>& anObservationArray,
    const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas
) const
{
    // Convert inputs to an inertial frame for estimation
    const State initialGuessStateInEstimationFrame = anInitialGuessState.inFrame(estimationFrameSPtr_);

    const Array<State> observationsInEstimationFrame = anObservationArray.map<State>(
        [estimationFrameSPtr = estimationFrameSPtr_](const State& aState) -> State
        {
            return aState.inFrame(estimationFrameSPtr);
        }
    );

    // Setup state builders

    // Propagation state builder: Contains the subsets of the provided initial guess state (all of which are needed to
    // propagate that state).
    const StateBuilder propagationStateBuilder(initialGuessStateInEstimationFrame);

    // Estimation state builder: Contains the subsets to be estimated.
    const StateBuilder estimationStateBuilder(
        initialGuessStateInEstimationFrame.accessFrame(),
        anEstimationCoordinateSubsets.isEmpty() ? initialGuessStateInEstimationFrame.getCoordinateSubsets()
                                                : anEstimationCoordinateSubsets
    );

    // Validate estimator subsets
    for (const auto& subset : estimationStateBuilder.getCoordinateSubsets())
    {
        if (!initialGuessStateInEstimationFrame.hasSubset(subset))
        {
            throw ostk::core::error::RuntimeError(
                "Input State must contain at least the coordinate subsets that we want to estimate."
            );
        }
    }

    // Define state generation callback
    auto generateStatesCallback = [&](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        const State propagatorState = propagationStateBuilder.expand(aState, initialGuessStateInEstimationFrame);
        return propagator_.calculateStatesAt(propagatorState, anInstantArray);
    };

    // Solve least squares problem
    const LeastSquaresSolver::Analysis analysis = estimator_.solve(
        estimationStateBuilder.reduce(initialGuessStateInEstimationFrame),
        observationsInEstimationFrame,
        generateStatesCallback,
        anInitialGuessSigmas,
        anObservationSigmas
    );

    // Expand solution state to full state
    const State estimatedState =
        propagationStateBuilder.expand(analysis.estimatedState, initialGuessStateInEstimationFrame)
            .inFrame(anInitialGuessState.accessFrame());

    return Analysis(estimatedState, analysis);
}

Orbit OrbitDeterminationSolver::estimateOrbit(
    const State& anInitialGuessState,
    const Array<State>& anObservationArray,
    const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas
) const
{
    const Analysis analysis = estimate(
        anInitialGuessState,
        anObservationArray,
        anEstimationCoordinateSubsets,
        anInitialGuessSigmas,
        anObservationSigmas
    );

    return Orbit(
        Propagated(this->propagator_, {analysis.estimatedState}), this->environment_.accessCentralCelestialObject()
    );
}

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
