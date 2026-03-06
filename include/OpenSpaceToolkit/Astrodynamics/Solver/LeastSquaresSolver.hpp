/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver__

#include <functional>
#include <unordered_map>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::time::Instant;

using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

#define DEFAULT_INITIAL_GUESS_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()  // Initial guess sigmas
#define DEFAULT_OBSERVATION_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()    // Observation sigmas
#define DEFAULT_FINITE_DIFFERENCE_SOLVER FiniteDifferenceSolver::Default()  // Default finite difference solver

/// @brief Class to solve non-linear least squares problems.
///
/// @details Implements a batch weighted least squares algorithm for orbit determination and
/// state estimation. Iteratively refines a state estimate by minimizing the weighted sum of
/// squared residuals between observations and computed values.
class LeastSquaresSolver
{
   public:
    using ScaleFactorGenerator = std::function<VectorXd(const State&)>;

    /// @brief A single iteration step of the least squares solver.
    class Step
    {
       public:
        /// @brief Constructor
        ///
        /// @param aRmsError RMS error for this step.
        /// @param anXHat State correction vector for this step.
        Step(const Real& aRmsError, const VectorXd& anXHat);

        /// @brief Stream step
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Step& aStep);

        /// @brief Print step
        void print(std::ostream& anOutputStream) const;

        Real rmsError;  ///< RMS error for this step.
        VectorXd xHat;  ///< State correction vector for this step.
    };

    /// @brief Analysis results from the least squares solver.
    class Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param aTerminationCriteria Termination criteria description.
        /// @param anEstimatedState Estimated state.
        /// @param anEstimatedCovariance Estimated covariance matrix.
        /// @param anEstimatedFrisbeeCovariance Estimated Frisbee covariance matrix.
        /// @param aComputedObservationsStateArray Array of computed observation states.
        /// @param aStepArray Array of solver iteration steps.
        Analysis(
            const String& aTerminationCriteria,
            const State& anEstimatedState,
            const MatrixXd& anEstimatedCovariance,
            const MatrixXd& anEstimatedFrisbeeCovariance,
            const Array<State>& aComputedObservationsStateArray,
            const Array<Step>& aStepArray
        );

        /// @brief Stream analysis
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief computeResidualStates
        Array<State> computeResidualStates(const Array<State>& anObservationStateArray) const;

        Real rmsError;                           ///< RMS error of the solution.
        Size observationCount;                   ///< Number of observations used.
        Size iterationCount;                     ///< Number of iterations performed.
        String terminationCriteria;              ///< Description of why the solver terminated.
        State estimatedState;                    ///< Estimated state at the solution.
        MatrixXd estimatedCovariance;            ///< Estimated covariance matrix of the solution.
        MatrixXd estimatedFrisbeeCovariance;     ///< Estimated Frisbee covariance matrix of the solution.
        Array<State> computedObservationStates;  ///< Array of computed observation states at the solution.
        Array<Step> steps;                       ///< Array of solver iteration steps.
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     LeastSquaresSolver solver = { 10, 1e-3 } ;
    /// @endcode
    ///
    /// @param aMaxIterationCount Maximum number of iterations
    /// @param aRmsUpdateThreshold Minimum RMS threshold
    /// @param aFiniteDifferenceSolver Finite difference solver. Defaults to FiniteDifferenceSolver::Default()
    LeastSquaresSolver(
        const Size& aMaxIterationCount,
        const Real& aRmsUpdateThreshold,
        const FiniteDifferenceSolver& aFiniteDifferenceSolver = DEFAULT_FINITE_DIFFERENCE_SOLVER
    );

    /// @brief Constructor with custom scale factor generator
    ///
    /// @code{.cpp}
    ///     auto solver = LeastSquaresSolver(
    ///         20, 1.0, FiniteDifferenceSolver.default(), LeastSquaresSolver.max_absolute_coordinate_scaling()
    ///     );
    /// @endcode
    ///
    /// @param aMaxIterationCount Maximum number of iterations
    /// @param aRmsUpdateThreshold Minimum RMS threshold
    /// @param aFiniteDifferenceSolver Finite difference solver
    /// @param aScaleFactorGenerator Function to generate scale factors from a state
    LeastSquaresSolver(
        const Size& aMaxIterationCount,
        const Real& aRmsUpdateThreshold,
        const FiniteDifferenceSolver& aFiniteDifferenceSolver,
        const ScaleFactorGenerator& aScaleFactorGenerator
    );

    /// @brief Get max iteration count
    ///
    /// @return Max iteration count
    Size getMaxIterationCount() const;

    /// @brief Get RMS update threshold
    ///
    /// @return RMS update threshold
    Real getRmsUpdateThreshold() const;

    /// @brief Get finite difference solver
    ///
    /// @return Finite difference solver
    FiniteDifferenceSolver getFiniteDifferenceSolver() const;

    /// @brief Get the scale factor generator
    ///
    /// @code{.cpp}
    ///     auto generator = solver.getScaleFactorGenerator();
    /// @endcode
    ///
    /// @return Scale factor generator function
    ScaleFactorGenerator getScaleFactorGenerator() const;

    /// @brief Solve the non-linear least squares problem
    /// Ref: https://www.sciencedirect.com/book/9780126836301/statistical-orbit-determination (Chapter 4, pg 196 for
    /// algorithm used)
    ///
    /// @param anInitialGuessState Initial guess state (the Estimated State is of the same domain as this state)
    /// @param anObservationStateArray Array of observation states
    /// @param aStateGenerator Function to generate states
    /// @param anInitialGuessSigmas Dictionary of sigmas for initial guess
    /// @param anObservationSigmas Dictionary of sigmas for observations
    ///
    /// @return Analysis
    Analysis solve(
        const State& anInitialGuessState,
        const Array<State>& anObservationStateArray,
        const std::function<Array<State>(const State&, const Array<Instant>&)>& aStateGenerator,
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas = DEFAULT_OBSERVATION_SIGMAS
    ) const;

    /// @brief Calculate empirical covariance
    ///
    /// @param aResidualStateArray Array of residual states
    ///
    /// @return Empirical covariance
    static MatrixXd calculateEmpiricalCovariance(const Array<State>& aResidualStateArray);

    /// @brief Create a scale factor generator that returns all ones (no scaling)
    ///
    /// @code{.cpp}
    ///     auto generator = LeastSquaresSolver.no_scaling();
    /// @endcode
    ///
    /// @return ScaleFactorGenerator that performs no scaling
    static ScaleFactorGenerator NoScaling();

    /// @brief Create a scale factor generator that uses max absolute coordinate values
    ///
    /// @code{.cpp}
    ///     auto generator = LeastSquaresSolver.max_absolute_coordinate_scaling();
    /// @endcode
    ///
    /// @return ScaleFactorGenerator that scales by max(|coord_i|, 1e-8)
    static ScaleFactorGenerator MaxAbsoluteCoordinateScaling();

    /// @brief Default constructor
    ///
    /// @code{.cpp}
    ///     LeastSquaresSolver solver = LeastSquaresSolver::Default() ;
    /// @endcode
    ///
    /// @return A default instance
    static LeastSquaresSolver Default();

   private:
    Size maxIterationCount_;
    Real rmsUpdateThreshold_;
    FiniteDifferenceSolver finiteDifferenceSolver_;
    ScaleFactorGenerator scaleFactorGenerator_;

    /// @brief Extract the inverse squares of the sigmas
    ///
    /// @param aSigmas Dictionary of sigmas
    /// @param aStateBuilder State builder
    ///
    /// @return Diagonal matrix of sigmas
    static MatrixXd extractInverseSquaredSigmas(
        const std::unordered_map<CoordinateSubset, VectorXd>& aSigmas, const StateBuilder& aStateBuilder
    );
};

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk

#endif
