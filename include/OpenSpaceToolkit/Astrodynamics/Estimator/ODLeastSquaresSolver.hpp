/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_ODLeastSquaresSolver__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_ODLeastSquaresSolver__

#include <unordered_map>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::NumericalSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Orbit Determination solver using least squares
class ODLeastSquaresSolver
{
   public:
    class Analysis
    {
       public:
        /// @brief Constructor
        Analysis(const State& aDeterminedState, const LeastSquaresSolver::Analysis& anAnalysis);

        /// @brief Destructor
        ~Analysis() = default;

        /// @brief Print analysis
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream) const;

        /// @brief Get determined state
        const State& getDeterminedState() const;

        /// @brief Get analysis
        const LeastSquaresSolver::Analysis& getSolverAnalysis() const;

       private:
        State determinedState_;
        LeastSquaresSolver::Analysis solverAnalysis_;
    };

    static const Shared<const Frame> EstimationFrameSPtr;

    /// @brief Constructor
    ///
    /// @param anEnvironment Environment
    /// @param aNumericalSolver Numerical solver
    /// @param aSolver Least squares solver
    ODLeastSquaresSolver(
        const Environment& anEnvironment,
        const NumericalSolver& aNumericalSolver = NumericalSolver::Default(),
        const LeastSquaresSolver& aSolver = LeastSquaresSolver::Default()
    );

    /// @brief Access environment
    const Environment& accessEnvironment() const;

    /// @brief Access propagator configuration
    const Propagator& accessPropagator() const;

    /// @brief Access solver
    const LeastSquaresSolver& accessSolver() const;

    /// @brief Estimate state using least squares
    ///
    /// @param anInitialGuessState Initial guess state
    /// @param aReferenceStateArray Reference states to minimize error against
    /// @param anInitialGuessSigmas Initial guess sigmas
    /// @param aReferenceStateSigmas Reference state sigmas
    /// @param anEstimationCoordinateSubsets Coordinate subsets to estimate
    Analysis estimateState(
        const State& anInitialGuessState,
        const Array<State>& aReferenceStateArray,
        const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets =
            Array<Shared<const CoordinateSubset>>::Empty(),
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& aReferenceStateSigmas = DEFAULT_REFERENCE_STATE_SIGMAS
    ) const;

    /// @brief Estimate orbit
    ///
    /// @param anInitialGuessState Initial guess state
    /// @param aReferenceStateArray Reference states
    /// @param anInitialGuessSigmas Initial guess sigmas
    /// @param aReferenceStateSigmas Reference state sigmas
    /// @param anEstimationCoordinateSubsets Coordinate subsets to estimate
    Orbit estimateOrbit(
        const State& anInitialGuessState,
        const Array<State>& aReferenceStateArray,
        const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets =
            Array<Shared<const CoordinateSubset>>::Empty(),
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& aReferenceStateSigmas = DEFAULT_REFERENCE_STATE_SIGMAS
    ) const;

   private:
    Environment environment_;
    Propagator propagator_;
    LeastSquaresSolver solver_;
};

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
