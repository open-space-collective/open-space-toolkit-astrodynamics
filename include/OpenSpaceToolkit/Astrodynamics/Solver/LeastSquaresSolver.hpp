/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver__

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

#define DEFAULT_INITIAL_GUESS_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()    // Initial guess sigmas
#define DEFAULT_REFERENCE_STATE_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()  // Reference state sigmas
#define DEFAULT_FINITE_DIFFERENCE_SOLVER FiniteDifferenceSolver::Default()  // Default finite difference solver

/// @brief Class to solve non-linear least squares problems
class LeastSquaresSolver
{
   public:
    class Step
    {
       public:
        /// @brief Constructor
        Step(const Real& aRmsError, const VectorXd& anXHat);

        /// @brief Destructor
        ~Step() = default;

        /// @brief Stream step
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Step& aStep);

        /// @brief Print step
        void print(std::ostream& anOutputStream) const;

        Real rmsError;
        VectorXd xHat;
    };

    class Analysis
    {
       public:
        /// @brief Constructor
        Analysis(
            const Real& aRmsError,
            const Size& anIterationCount,
            const String& aTerminationCriteria,
            const State& aSolutionState,
            const MatrixXd& aSolutionCovariance,
            const MatrixXd& aSolutionFrisbeeCovariance,
            const Array<Step>& steps
        );

        /// @brief Destructor
        ~Analysis() = default;

        /// @brief Stream analysis
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        Real rmsError;
        Size iterationCount;
        String terminationCriteria;
        State solutionState;
        MatrixXd solutionCovariance;
        MatrixXd solutionFrisbeeCovariance;
        Array<Step> steps;
    };

    /// @brief Constructor
    ///
    /// @param aMaxIterationCount Maximum number of iterations
    /// @param aRmsUpdateThreshold Minimum RMS threshold
    /// @param aFiniteDifferenceSolver Finite difference solver. Defaults to FiniteDifferenceSolver::Default()
    LeastSquaresSolver(
        const Size& aMaxIterationCount,
        const Real& aRmsUpdateThreshold,
        const FiniteDifferenceSolver& aFiniteDifferenceSolver = DEFAULT_FINITE_DIFFERENCE_SOLVER
    );

    /// @brief Get max iteration count
    Size getMaxIterationCount() const;

    /// @brief Get RMS update threshold
    Real getRmsUpdateThreshold() const;

    /// @brief Get finite difference solver
    FiniteDifferenceSolver getFiniteDifferenceSolver() const;

    /// @brief Solve the non-linear least squares problem
    ///
    /// @param anInitialGuessState Initial guess state
    /// @param aReferenceStateArray Array of reference states
    /// @param anInitialGuessSigmas Dictionary of sigmas for initial guess
    /// @param aReferenceStateSigmas Dictionary of sigmas for reference states
    /// @param aGenerateStatesCallback Function to generate states
    Analysis solve(
        const State& anInitialGuessState,
        const Array<State>& aReferenceStateArray,
        const std::function<Array<State>(const State&, const Array<Instant>&)>& aGenerateStatesCallback,
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& aReferenceStateSigmas = DEFAULT_REFERENCE_STATE_SIGMAS
    ) const;

    /// @brief Calculate empirical covariance
    ///
    /// @param aResidualArray Array of residuals
    static MatrixXd calculateEmpiricalCovariance(const Array<State>& aResidualStateArray);

    /// @brief Default constructor
    ///
    /// @return A default instance
    static LeastSquaresSolver Default();

   private:
    Size maxIterationCount_;
    Real rmsUpdateThreshold_;
    FiniteDifferenceSolver finiteDifferenceSolver_;

    /// @brief Extract the sigmas
    ///
    /// @param aSigmas Dictionary of sigmas
    /// @param aStateBuilder State builder
    static VectorXd extractSigmas(
        const std::unordered_map<CoordinateSubset, VectorXd>& aSigmas, const StateBuilder& aStateBuilder
    );
};

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk

#endif