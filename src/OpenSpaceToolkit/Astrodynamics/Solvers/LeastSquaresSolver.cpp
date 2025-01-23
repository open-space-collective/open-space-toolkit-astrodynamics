/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
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
    : rmsError_(aRmsError),
      xHat_(anXHat)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const LeastSquaresSolver::Step& aStep)
{
    aStep.print(anOutputStream);

    return anOutputStream;
}

void LeastSquaresSolver::Step::print(std::ostream& anOutputStream) const
{
    ostk::core::utils::Print::Line(anOutputStream) << "RMS Error: " << rmsError_;
    ostk::core::utils::Print::Line(anOutputStream) << "X Hat: " << xHat_.toString(4);
}

Real LeastSquaresSolver::Step::getRmsError() const
{
    return rmsError_;
}

VectorXd LeastSquaresSolver::Step::getXHat() const
{
    return xHat_;
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
    : rmsError_(aRmsError),
      iterationCount_(anIterationCount),
      terminationCriteria_(aTerminationCriteria),
      solutionState_(aSolutionState),
      solutionCovariance_(aSolutionCovariance),
      solutionFrisbeeCovariance_(aSolutionFrisbeeCovariance),
      steps_(aStepArray)
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

    ostk::core::utils::Print::Line(anOutputStream) << "RMS Error: " << rmsError_;
    ostk::core::utils::Print::Line(anOutputStream) << "Iteration Count: " << iterationCount_;
    ostk::core::utils::Print::Line(anOutputStream) << "Termination Criteria: " << terminationCriteria_;

    ostk::core::utils::Print::Separator(anOutputStream, "Solution State");
    solutionState_.print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Steps");
    for (const auto& step : steps_)
    {
        step.print(anOutputStream);
    }

    ostk::core::utils::Print::Footer(anOutputStream);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real LeastSquaresSolver::Analysis::getRmsError() const
{
    return rmsError_;
}

Size LeastSquaresSolver::Analysis::getIterationCount() const
{
    return iterationCount_;
}

String LeastSquaresSolver::Analysis::getTerminationCriteria() const
{
    return terminationCriteria_;
}

const State& LeastSquaresSolver::Analysis::accessSolutionState() const
{
    return solutionState_;
}

const MatrixXd& LeastSquaresSolver::Analysis::accessSolutionCovariance() const
{
    return solutionCovariance_;
}

const MatrixXd& LeastSquaresSolver::Analysis::accessSolutionFrisbeeCovariance() const
{
    return solutionFrisbeeCovariance_;
}

const Array<LeastSquaresSolver::Step>& LeastSquaresSolver::Analysis::accessSteps() const
{
    return steps_;
}

LeastSquaresSolver::LeastSquaresSolver(const Size& aMaxIterationCount, const Real& aRmsUpdateThreshold)
    : maxIterationCount_(aMaxIterationCount),
      rmsUpdateThreshold_(aRmsUpdateThreshold)
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

    for (const auto& state : aReferenceStateArray)
    {
        if (state.getCoordinateSubsets() != referenceStateSubsets)
        {
            throw ostk::core::error::RuntimeError("Reference states must have the same coordinate subsets.");
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
            "Reference State count * reference State dimension should be greater than estimated State dimension."
        );
    }

    // Setup a priori and covariance matrices
    VectorXd xApriori = VectorXd::Zero(stateDimension);
    MatrixXd PAprioriInv;

    if (!anInitialGuessSigmas.empty())
    {
        const VectorXd sigmas = extractSigmas(anInitialGuessSigmas, estimationStateBuilder);
        PAprioriInv = sigmas.array().square().inverse().matrix().asDiagonal();
    }
    else
    {
        PAprioriInv = MatrixXd::Zero(stateDimension, stateDimension);
    }

    // Setup measurement covariance matrix
    MatrixXd RInv;
    if (!aReferenceStateSigmas.empty())
    {
        const VectorXd sigmas = extractSigmas(aReferenceStateSigmas, referenceStateBuilder);
        RInv = sigmas.array().square().inverse().matrix().asDiagonal();
    }
    else
    {
        RInv = MatrixXd::Identity(referenceStateDimension, referenceStateDimension);
    }

    // Initialize state vectors
    VectorXd XNom = anInitialGuessState.accessCoordinates();
    VectorXd xHat = VectorXd::Zero(stateDimension);

    // Get reference state coordinates
    MatrixXd referenceStatesCoordinates(referenceStateCount, referenceStateDimension);
    for (Size i = 0; i < referenceStateCount; ++i)
    {
        referenceStatesCoordinates.row(i) = aReferenceStateArray[i].inFrame(estimationStateFrame).accessCoordinates();
    }

    // Initialize arrays for the iteration
    MatrixXd residuals = MatrixXd::Zero(referenceStateCount, referenceStateDimension);
    Real previousRmsError = std::numeric_limits<double>::max();
    String terminationCriteria = "Maximum Iteration Threshold";
    Array<Step> steps;

    MatrixXd Lambda;
    MatrixXd frisbeeCovariance = MatrixXd::Zero(stateDimension, stateDimension);

    const FiniteDifferenceSolver finiteDifferenceSolver = FiniteDifferenceSolver::Default();

    // Main iteration loop
    for (Size iteration = 0; iteration < maxIterationCount_; ++iteration)
    {
        // Initialize matrices that will be accumulated in the loop
        Lambda = PAprioriInv;                 // Λ = P̄⁻¹
        VectorXd N = PAprioriInv * xApriori;  // N = P̄⁻¹ x̄
        frisbeeCovariance = MatrixXd::Zero(stateDimension, stateDimension);

        // Generate states at reference instants
        Array<Instant> referenceInstants;
        for (const auto& state : aReferenceStateArray)
        {
            referenceInstants.add(state.getInstant());
        }

        // Build current state for propagation
        const State currentState = estimationStateBuilder.build(anInitialGuessState.getInstant(), XNom);
        const Array<State> computedStates = aGenerateStatesCallback(currentState, referenceInstants);

        // Convert computed states to matrix form
        MatrixXd computedStatesCoordinates(referenceStateCount, referenceStateDimension);
        for (Size i = 0; i < referenceStateCount; ++i)
        {
            computedStatesCoordinates.row(i) =
                computedStates[i].inFrame(estimationStateFrame).extractCoordinates(referenceStateSubsets);
        }

        // Compute Jacobian matrix using finite differences
        const Array<MatrixXd> HFull = finiteDifferenceSolver.computeStateTransitionMatrix(
            currentState,
            referenceInstants,
            [&](const State& state, const Array<Instant>& instants) -> MatrixXd
            {
                const Array<State> states = aGenerateStatesCallback(state, instants);
                MatrixXd coordinates(referenceStateDimension, referenceStateCount);
                for (Size i = 0; i < states.getSize(); ++i)
                {
                    coordinates.col(i) =
                        states[i].inFrame(estimationStateFrame).extractCoordinates(referenceStateSubsets);
                }
                return coordinates;
            }
        );

        // Loop through each reference state
        for (Size i = 0; i < referenceStateCount; ++i)
        {
            // Compute residual
            const VectorXd y =
                referenceStatesCoordinates.row(i).transpose() - computedStatesCoordinates.row(i).transpose();
            residuals.row(i) = y.transpose();

            // Extract current H matrix
            const MatrixXd H = HFull[i];

            // Accumulate normal equations
            Lambda += H.transpose() * RInv * H;
            N += H.transpose() * RInv * y;

            // Accumulate Frisbee covariance
            frisbeeCovariance += H.transpose() * RInv * y * y.transpose() * RInv * H;
        }

        // Solve normal equations
        xHat = Lambda.ldlt().solve(N);

        // Update state vector and a priori deviation
        XNom += xHat;
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
    const MatrixXd PHat = Lambda.ldlt().solve(MatrixXd::Identity(Lambda.rows(), Lambda.cols()));
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
