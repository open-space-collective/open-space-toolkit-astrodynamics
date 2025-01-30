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
    const Size& anObservationCount,
    const String& aTerminationCriteria,
    const State& aSolutionState,
    const MatrixXd& aSolutionCovariance,
    const MatrixXd& aSolutionFrisbeeCovariance,
    const MatrixXd& aSolutionResidualMatrix,
    const Array<Step>& aStepArray
)
    : rmsError(std::sqrt(aSolutionResidualMatrix.colwise().norm().array().square().sum() / anObservationCount)),
      observationCount(anObservationCount),
      iterationCount(aStepArray.getSize()),
      terminationCriteria(aTerminationCriteria),
      solutionState(aSolutionState),
      solutionCovariance(aSolutionCovariance),
      solutionFrisbeeCovariance(aSolutionFrisbeeCovariance),
      solutionResiduals(aSolutionResidualMatrix),
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
    const Array<State>& anObservationArray,
    const std::function<Array<State>(const State&, const Array<Instant>&)>& aStateGenerator,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas
) const
{
    // Notation used: https://www.sciencedirect.com/book/9780126836301/statistical-orbit-determination (Chapter 4, pg 196 for algorithm used)

    if (!anInitialGuessState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Initial guess state");
    }

    if (anObservationArray.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Observations");
    }

    // Setup state builders
    const StateBuilder estimationStateBuilder(anInitialGuessState);
    const Shared<const Frame>& estimationStateFrame = anInitialGuessState.accessFrame();
    const StateBuilder observationBuilder(anObservationArray[0]);
    const Array<Shared<const CoordinateSubset>> observationSubsets = observationBuilder.getCoordinateSubsets();

    for (const auto& observation : anObservationArray)
    {
        if (observation.getCoordinateSubsets() != observationSubsets)
        {
            throw ostk::core::error::RuntimeError("Observations must have the same coordinate subsets.");
        }
        if (observation.getFrame() != observationBuilder.getFrame())
        {
            throw ostk::core::error::RuntimeError("Observations must have the same frame.");
        }
    }

    // Get dimensions
    const Size stateDimension = anInitialGuessState.accessCoordinates().size();
    const Size observationDimension = anObservationArray[0].accessCoordinates().size();
    const Size observationCount = anObservationArray.size();

    // Validate dimensions
    if (observationDimension * observationCount <= stateDimension)
    {
        throw ostk::core::error::RuntimeError(
            "Observation count * observation dimension should be greater than estimated State dimension to "
            "yield a full rank H matrix."
        );
    }

    // Setup a priori and covariance matrices
    // x̄
    VectorXd xApriori = VectorXd::Zero(stateDimension);

    // P̄⁻¹ = diag(1/σ²)
    const MatrixXd PAprioriInverse = anInitialGuessSigmas.empty()
                                       ? MatrixXd::Zero(stateDimension, stateDimension)
                                       : extractSigmas(anInitialGuessSigmas, estimationStateBuilder);

    // Setup measurement covariance matrix
    // R⁻¹ = diag(1/σ²)
    const MatrixXd RInv = anObservationSigmas.empty() ? MatrixXd::Identity(observationDimension, observationDimension)
                                                      : extractSigmas(anObservationSigmas, observationBuilder);

    // Initialize state vectors
    // X∗ (nominal trajectory)
    VectorXd XNom = anInitialGuessState.accessCoordinates();
    // x̂ = X - X∗
    VectorXd xHat = VectorXd::Zero(stateDimension);

    // Get observations coordinates
    // Y
    MatrixXd observationCoordinates(observationDimension, observationCount);
    for (Size i = 0; i < observationCount; ++i)
    {
        observationCoordinates.col(i) = anObservationArray[i].inFrame(estimationStateFrame).accessCoordinates();
    }

    // Initialize arrays for the iteration
    MatrixXd residuals = MatrixXd::Zero(observationDimension, observationCount);
    Real previousRmsError = 0.0;
    String terminationCriteria = "Maximum Iteration Threshold";
    Array<Step> steps;

    MatrixXd Lambda = MatrixXd::Zero(stateDimension, stateDimension);
    MatrixXd PHatFrisbee = MatrixXd::Zero(stateDimension, stateDimension);

    const Array<Instant> observationInstants = anObservationArray.map<Instant>(
        [](const auto& state) -> Instant
        {
            return state.getInstant();
        }
    );

    State currentEstimatedState = State::Undefined();

    const auto computeObservationsCoordinates = [&](const State& state, const Array<Instant>& instants) -> MatrixXd
    {
        const Array<State> states = aStateGenerator(state, instants);

        MatrixXd coordinates(observationDimension, observationCount);

        for (Size i = 0; i < states.getSize(); ++i)
        {
            coordinates.col(i) = states[i].inFrame(estimationStateFrame).extractCoordinates(observationSubsets);
        }
        return coordinates;
    };

    // Main iteration loop
    for (Size iteration = 0; iteration < maxIterationCount_; ++iteration)
    {
        // Initialize matrices that will be accumulated in the loop
        Lambda = PAprioriInverse;                 // Λ = P̄⁻¹ (shorthand for Hᵀ R⁻¹ H)
        VectorXd N = PAprioriInverse * xApriori;  // N = P̄⁻¹ x̄ (shorthand for H^T R^-1 y)

        // Initialize the Pₑ of equation 19 in https://ntrs.nasa.gov/citations/20140011726
        PHatFrisbee = MatrixXd::Zero(stateDimension, stateDimension);

        currentEstimatedState = estimationStateBuilder.build(anInitialGuessState.getInstant(), XNom);
        
        // G(X∗ᵢ) (computed observations) for all observation instants
        const MatrixXd computedStatesCoordinates =
            computeObservationsCoordinates(currentEstimatedState, observationInstants);

        // Compute residuals
        // y = Y - G(X∗) (observed - computed = residuals)
        residuals = observationCoordinates - computedStatesCoordinates;

        // H(t,t₀) = ∂G(X∗)/∂X∗₀ (sensitivty matrix for all observations at tᵢ w.r.t. nominal trajectory at epoch
        // t₀)
        const Array<MatrixXd> HFull = finiteDifferenceSolver_.computeStateTransitionMatrix(
            currentEstimatedState, observationInstants, computeObservationsCoordinates
        );

        // Loop through each observation
        for (Size i = 0; i < observationCount; ++i)
        {
            // yᵢ = Yᵢ - G(X∗ᵢ)
            const VectorXd& y = residuals.col(i);
            // Hᵢ = H(tᵢ,t₀) (observations sensitivity matrix for current observations)
            const MatrixXd& H = HFull[i];

            // Λ = Λ + (Hᵢᵀ R⁻¹ Hᵢ)
            Lambda += H.transpose() * RInv * H;

            // N = N + (Hᵢᵀ R⁻¹ yᵢ)
            N += H.transpose() * RInv * y;

            // Pₑ = Pₑ + Hᵢᵀ R⁻¹ yᵢ yᵢᵀ R⁻¹ Hᵢ
            PHatFrisbee += H.transpose() * RInv * y * y.transpose() * RInv * H;
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
        const Real currentRmsError = std::sqrt(residuals.colwise().norm().array().square().sum() / observationCount);

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
    PHatFrisbee = PHat * PHatFrisbee * PHat;

    return Analysis(observationCount, terminationCriteria, currentEstimatedState, PHat, PHatFrisbee, residuals, steps);
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

MatrixXd LeastSquaresSolver::extractSigmas(
    const std::unordered_map<CoordinateSubset, VectorXd>& aSigmas, const StateBuilder& aStateBuilder
)
{
    const Array<Shared<const CoordinateSubset>> stateSubsets = aStateBuilder.getCoordinateSubsets();

    if (aSigmas.size() != stateSubsets.getSize())
    {
        throw ostk::core::error::RuntimeError("Sigma count does not match State Coordinate Subset count.");
    }

    VectorXd sigmasCoordinates(aStateBuilder.getSize());
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

    return sigmasCoordinates.array().square().inverse().matrix().asDiagonal();
    ;
}

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk
