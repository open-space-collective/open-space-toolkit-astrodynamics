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
#include <OpenSpaceToolkit/Astrodynamics/Solver/Estimator.hpp>

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
using ostk::astrodynamics::estimator::Estimator;

#define DEFAULT_INITIAL_GUESS_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()  // Initial guess sigmas
#define DEFAULT_OBSERVATION_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()    // Observation sigmas
#define DEFAULT_FINITE_DIFFERENCE_SOLVER FiniteDifferenceSolver::Default()  // Default finite difference solver

/// @brief Class to solve non-linear least squares problems
class LeastSquaresSolver : public Estimator
{
   public:
    class Step
    {
       public:
        /// @brief Constructor
        Step(const Real& aRmsError, const VectorXd& anXHat);

        /// @brief Stream step
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Step& aStep);

        /// @brief Print step
        void print(std::ostream& anOutputStream) const;

        Real rmsError;
        VectorXd xHat;
    };

    class Analysis : public Estimator::Analysis
    {
       public:
        /// @brief Constructor
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

        Size iterationCount;
        String terminationCriteria;
        MatrixXd estimatedFrisbeeCovariance;
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
    virtual Analysis solve(
        const State& anInitialGuessState,
        const Array<State>& anObservationStateArray,
        const std::function<Array<State>(const State&, const Array<Instant>&)>& aStateGenerator,
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas = DEFAULT_OBSERVATION_SIGMAS
    ) const override;

    /// @brief Calculate empirical covariance
    ///
    /// @param aResidualStateArray Array of residual states
    ///
    /// @return Empirical covariance
    static MatrixXd calculateEmpiricalCovariance(const Array<State>& aResidualStateArray);

    /// @brief Default constructor
    ///
    /// @return A default instance
    static LeastSquaresSolver Default();

   private:
    Size maxIterationCount_;
    Real rmsUpdateThreshold_;
    FiniteDifferenceSolver finiteDifferenceSolver_;

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
