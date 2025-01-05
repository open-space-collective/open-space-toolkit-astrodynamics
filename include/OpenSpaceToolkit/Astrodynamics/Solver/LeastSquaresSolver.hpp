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

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

#define DEFAULT_INITIAL_GUESS_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()    // Initial guess sigmas
#define DEFAULT_REFERENCE_STATE_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()  // Reference state sigmas

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

        /// @brief Get RMS error
        Real getRmsError() const;

        /// @brief Get X hat
        VectorXd getXHat() const;

       private:
        Real rmsError_;
        VectorXd xHat_;
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

        /// @brief Access RMS error
        Real getRmsError() const;

        /// @brief Access iteration count
        Size getIterationCount() const;

        /// @brief Access termination criteria
        String getTerminationCriteria() const;

        /// @brief Access solution state
        const State& accessSolutionState() const;

        /// @brief Access solution covariance
        const MatrixXd& accessSolutionCovariance() const;

        /// @brief Access solution Frisbee covariance
        const MatrixXd& accessSolutionFrisbeeCovariance() const;

        /// @brief Access steps
        const Array<Step>& accessSteps() const;

       private:
        Real rmsError_;
        Size iterationCount_;
        String terminationCriteria_;
        State solutionState_;
        MatrixXd solutionCovariance_;
        MatrixXd solutionFrisbeeCovariance_;
        Array<Step> steps_;
    };

    /// @brief Constructor
    ///
    /// @param aMaxIterationCount Maximum number of iterations
    /// @param aRmsUpdateThreshold Minimum RMS threshold
    LeastSquaresSolver(const Size& aMaxIterationCount, const Real& aRmsUpdateThreshold);

    /// @brief Get max iteration count
    Size getMaxIterationCount() const;

    /// @brief Get RMS update threshold
    Real getRmsUpdateThreshold() const;

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
    static MatrixXd calculateEmpiricalCovariance(const Array<State>& aResidualArray);

    /// @brief Default constructor
    ///
    /// @return A default instance
    static LeastSquaresSolver Default();

   private:
    Size maxIterationCount_;
    Real rmsUpdateThreshold_;

    /// @brief Expand sigmas
    static VectorXd expandSigmas(
        const std::unordered_map<CoordinateSubset, VectorXd>& aSigmas, const StateBuilder& aStateBuilder
    );
};

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk

#endif
