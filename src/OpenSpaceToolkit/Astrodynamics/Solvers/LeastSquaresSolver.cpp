/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;

LeastSquaresSolver::Step::Step(const Real& aRmsError, const VectorXd& anXHat)
    : rmsError(aRmsError),
      xHat(anXHat)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const LeastSquaresSolver::Step& aStep)
{
    aStep.print(anOutputStream);

    return anOutputStream;
}

void LeastSquaresSolver::Step::print(std::ostream& anOutputStream) const
{
    ostk::core::utils::Print::Line(anOutputStream) << "RMS Error: " << rmsError;
    ostk::core::utils::Print::Line(anOutputStream) << "X Hat: " << xHat.toString(4);
}

LeastSquaresSolver::Analysis::Analysis(
    const Real& aRmsError,
    const Size& anIterationCount,
    const String& aTerminationCriteria,
    const State& aSolutionState,
    const MatrixXd& aSolutionCovariance,
    const MatrixXd& aSolutionFrisbeeCovariance,
    const Array<Step>& aStepArray
)
    : rmsError(aRmsError),
      iterationCount(anIterationCount),
      terminationCriteria(aTerminationCriteria),
      solutionState(aSolutionState),
      solutionCovariance(aSolutionCovariance),
      solutionFrisbeeCovariance(aSolutionFrisbeeCovariance),
      steps(aStepArray)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const LeastSquaresSolver::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void LeastSquaresSolver::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Least Squares Solver Analysis") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "RMS Error: " << rmsError;
    ostk::core::utils::Print::Line(anOutputStream) << "Iteration Count: " << iterationCount;
    ostk::core::utils::Print::Line(anOutputStream) << "Termination Criteria: " << terminationCriteria;

    ostk::core::utils::Print::Separator(anOutputStream, "Solution State");
    solutionState.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Steps");
    for (const auto& step : steps)
    {
        step.print(anOutputStream);
    }

    ostk::core::utils::Print::Footer(anOutputStream);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

LeastSquaresSolver::LeastSquaresSolver(
    const Size& aMaxIterationCount,
    const Real& aRmsUpdateThreshold,
    const FiniteDifferenceSolver& aFiniteDifferenceSolver
)
    : maxIterationCount_(aMaxIterationCount),
      rmsUpdateThreshold_(aRmsUpdateThreshold),
      finiteDifferenceSolver_(aFiniteDifferenceSolver)
{
    if (aMaxIterationCount == 0)
    {
        throw ostk::core::error::RuntimeError("Max iteration count must be greater than 0.");
    }

    if (aRmsUpdateThreshold <= 0.0)
    {
        throw ostk::core::error::RuntimeError("RMS update threshold must be greater than 0.");
    }
}

Size LeastSquaresSolver::getMaxIterationCount() const
{
    return maxIterationCount_;
}

Real LeastSquaresSolver::getRmsUpdateThreshold() const
{
    return rmsUpdateThreshold_;
}

FiniteDifferenceSolver LeastSquaresSolver::getFiniteDifferenceSolver() const
{
    return finiteDifferenceSolver_;
}

LeastSquaresSolver::Analysis LeastSquaresSolver::solve(
    const State& anInitialGuessState,
    const Array<State>& aReferenceStateArray,
    const std::function<Array<State>(const State&, const Array<Instant>&)>& aGenerateStatesCallback,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& aReferenceStateSigmas
) const
{
    if (!anInitialGuessState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Initial guess state");
    }

    if (aReferenceStateArray.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Reference states");
    }

    // Setup state builders
    const StateBuilder estimationStateBuilder(anInitialGuessState);
    const Shared<const Frame>& estimationStateFrame = anInitialGuessState.accessFrame();
    const StateBuilder referenceStateBuilder(aReferenceStateArray[0]);
    const Array<Shared<const CoordinateSubset>> referenceStateSubsets = referenceStateBuilder.getCoordinateSubsets();

    for (const auto& referenceState : aReferenceStateArray)
    {
        if (referenceState.getCoordinateSubsets() != referenceStateSubsets)
        {
            throw ostk::core::error::RuntimeError("Reference states must have the same coordinate subsets.");
        }
        if (referenceState.getFrame() != referenceStateBuilder.getFrame())
        {
            throw ostk::core::error::RuntimeError("Reference states must have the same frame.");
        }
    }

    // Get dimensions
    const Size stateDimension = anInitialGuessState.accessCoordinates().size();
    const Size referenceStateDimension = aReferenceStateArray[0].accessCoordinates().size();
    const Size referenceStateCount = aReferenceStateArray.size();

    // Validate dimensions
    if (referenceStateDimension * referenceStateCount <= stateDimension)
    {
        throw ostk::core::error::RuntimeError(
            "Reference State count * reference State dimension should be greater than estimated State dimension to yield a full rank H matrix."
        );
    }

    // Setup a priori and covariance matrices
    // x̄
    VectorXd xApriori = VectorXd::Zero(stateDimension);

    // P̄⁻¹ = diag(1/σ²)
    MatrixXd PAprioriInverse = MatrixXd::Zero(stateDimension, stateDimension);
    if (!anInitialGuessSigmas.empty())
    {
        const VectorXd sigmas = extractSigmas(anInitialGuessSigmas, estimationStateBuilder);
        PAprioriInverse = sigmas.array().square().inverse().matrix().asDiagonal();
    }

    // Setup measurement covariance matrix
    // R⁻¹ = diag(1/σ²)
    MatrixXd RInv = MatrixXd::Identity(referenceStateDimension, referenceStateDimension);
    if (!aReferenceStateSigmas.empty())
    {
        const VectorXd sigmas = extractSigmas(aReferenceStateSigmas, referenceStateBuilder);
        RInv = sigmas.array().square().inverse().matrix().asDiagonal();
    }

    // Initialize state vectors
    // X∗ (nominal trajectory)
    VectorXd XNom = anInitialGuessState.accessCoordinates();
    // x̂ = X - X∗
    VectorXd xHat = VectorXd::Zero(stateDimension);

    // Get reference state coordinates
    // Y
    MatrixXd referenceStatesCoordinates(referenceStateDimension, referenceStateCount);
    for (Size i = 0; i < referenceStateCount; ++i)
    {
        referenceStatesCoordinates.col(i) = aReferenceStateArray[i].inFrame(estimationStateFrame).accessCoordinates();
    }

    // Initialize arrays for the iteration
    MatrixXd residuals = MatrixXd::Zero(referenceStateCount, referenceStateDimension);
    Real previousRmsError = 0.0;
    String terminationCriteria = "Maximum Iteration Threshold";
    Array<Step> steps;

    MatrixXd Lambda = MatrixXd::Zero(stateDimension, stateDimension);
    MatrixXd frisbeeCovariance = MatrixXd::Zero(stateDimension, stateDimension);

    // Generate states at reference instants
    Array<Instant> referenceInstants = aReferenceStateArray.map<Instant>(
        [](const auto& state) -> Instant
        {
            return state.getInstant();
        }
    );

    // Main iteration loop
    for (Size iteration = 0; iteration < maxIterationCount_; ++iteration)
    {
        // Initialize matrices that will be accumulated in the loop
        Lambda = PAprioriInverse;                 // Λ = P̄⁻¹
        VectorXd N = PAprioriInverse * xApriori;  // N = P̄⁻¹ x̄

        // Initialize the Pₑ of equation 19 in https://ntrs.nasa.gov/citations/20140011726
        frisbeeCovariance = MatrixXd::Zero(stateDimension, stateDimension);

        // Build current state for propagation
        // G(X∗ᵢ) (computed reference state) for all reference instants
        const State currentState = estimationStateBuilder.build(anInitialGuessState.getInstant(), XNom);
        const Array<State> esimatedStates = aGenerateStatesCallback(currentState, referenceInstants);

        // Convert computed states to matrix form
        MatrixXd computedStatesCoordinates(referenceStateDimension, referenceStateCount);
        for (Size i = 0; i < referenceStateCount; ++i)
        {
            computedStatesCoordinates.col(i) =
                esimatedStates[i].inFrame(estimationStateFrame).extractCoordinates(referenceStateSubsets);
        }

        // Compute residuals matrix
        const MatrixXd residualMatrix = referenceStatesCoordinates - computedStatesCoordinates;

        // H(t,t₀) = ∂G(X∗)/∂X∗₀ (sensitivty matrix for all reference states at tᵢ w.r.t. nominal trajectory at epoch
        // t₀)
        const auto computeReferenceStateCoordinates = [&](const State& state,
                                                          const Array<Instant>& instants) -> MatrixXd
        {
            const Array<State> states = aGenerateStatesCallback(state, instants);
            MatrixXd coordinates(referenceStateDimension, referenceStateCount);
            for (Size i = 0; i < states.getSize(); ++i)
            {
                coordinates.col(i) = states[i].inFrame(estimationStateFrame).extractCoordinates(referenceStateSubsets);
            }
            return coordinates;
        };

        const Array<MatrixXd> HFull = finiteDifferenceSolver_.computeStateTransitionMatrix(
            currentState, referenceInstants, computeReferenceStateCoordinates
        );

        // Loop through each reference state
        for (Size i = 0; i < referenceStateCount; ++i)
        {
            // Compute residual
            // yᵢ = Yᵢ - G(X∗ᵢ) (observed - computed = residual for current reference state)
            const VectorXd y = residualMatrix.col(i);
            residuals.row(i) = y;

            // Hᵢ = H(tᵢ,t₀) (reference state sensitivity matrix for current reference state)
            const MatrixXd H = HFull[i];

            // Λ = Λ + (Hᵢᵀ R⁻¹ Hᵢ)
            Lambda += H.transpose() * RInv * H;

            // N = N + (Hᵢᵀ R⁻¹ yᵢ)
            N += H.transpose() * RInv * y;

            // Pₑ = Pₑ + Hᵢᵀ R⁻¹ yᵢ yᵢᵀ R⁻¹ Hᵢ
            frisbeeCovariance += H.transpose() * RInv * y * y.transpose() * RInv * H;
        }

        // Solve normal equations
        // x̂ = Λ⁻¹ N
        xHat = Lambda.ldlt().solve(N);

        // Update state vector and a priori deviation
        // X∗ = X∗ + x̂
        XNom += xHat;

        // x̄ = x̄ - x̂
        xApriori -= xHat;

        // Compute current RMS error
        const Real currentRmsError = std::sqrt(residuals.rowwise().norm().array().square().sum() / referenceStateCount);

        // Store step information
        steps.add(Step(currentRmsError, xHat));

        // Check convergence
        if (std::abs(currentRmsError - previousRmsError) < rmsUpdateThreshold_)
        {
            terminationCriteria = "RMS Update Threshold";
            break;
        }

        previousRmsError = currentRmsError;
    }

    // Compute final covariance matrices
    // P̂ = Λ⁻¹
    const MatrixXd PHat = Lambda.ldlt().solve(MatrixXd::Identity(Lambda.rows(), Lambda.cols()));

    // Pₑ = P̂ Pₑ P̂
    frisbeeCovariance = PHat * frisbeeCovariance * PHat;

    return Analysis(
        previousRmsError,
        steps.getSize(),
        terminationCriteria,
        estimationStateBuilder.build(anInitialGuessState.getInstant(), XNom),
        PHat,
        frisbeeCovariance,
        steps
    );
}

MatrixXd LeastSquaresSolver::calculateEmpiricalCovariance(const Array<State>& aResidualArray)
{
    if (aResidualArray.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Residual array");
    }

    const Size dimension = aResidualArray[0].accessCoordinates().size();
    const Size count = aResidualArray.getSize();

    MatrixXd coordinates(count, dimension);
    for (Size i = 0; i < count; ++i)
    {
        coordinates.row(i) = aResidualArray[i].accessCoordinates();
    }

    return (coordinates.transpose() * coordinates) / count;
}

LeastSquaresSolver LeastSquaresSolver::Default()
{
    return LeastSquaresSolver(20, 1.0);
}

VectorXd LeastSquaresSolver::extractSigmas(
    const std::unordered_map<CoordinateSubset, VectorXd>& aSigmas, const StateBuilder& aStateBuilder
)
{
    const Array<Shared<const CoordinateSubset>> stateSubsets = aStateBuilder.getCoordinateSubsets();

    if (aSigmas.size() != stateSubsets.getSize())
    {
        throw ostk::core::error::RuntimeError("Sigma count does not match State Coordinate Subset count.");
    }

    Size totalSize = 0;
    for (const auto& subset : stateSubsets)
    {
        totalSize += subset->getSize();
    }

    VectorXd sigmasCoordinates(totalSize);
    Size currentIndex = 0;

    for (Size i = 0; i < stateSubsets.getSize(); ++i)
    {
        const auto& subset = stateSubsets[i];
        const Size subsetSize = subset->getSize();
        const auto sigmaIt = aSigmas.find(*subset);

        if (sigmaIt == aSigmas.end())
        {
            throw ostk::core::error::RuntimeError("Sigma Coordinate Subset does not match State Coordinate Subset.");
        }

        if ((sigmaIt->second.array() <= 0.0).any())
        {
            throw ostk::core::error::RuntimeError("Sigma value must be greater than 0.");
        }

        const VectorXd scaledSigma = sigmaIt->second / std::sqrt(subsetSize);
        sigmasCoordinates.segment(currentIndex, subsetSize) = scaledSigma;
        currentIndex += subsetSize;
    }

    return sigmasCoordinates;
}

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk