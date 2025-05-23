/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_OrbitDeterminationSolver__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_OrbitDeterminationSolver__

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
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::NumericalSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

#define DEFAULT_ENVIRONMENT Environment::Default()                  // Default environment
#define DEFAULT_NUMERICAL_SOLVER NumericalSolver::Default()         // Default numerical solver
#define DEFAULT_LEAST_SQUARES_SOLVER LeastSquaresSolver::Default()  // Default least squares solver
#define DEFAULT_ESTIMATION_FRAME Frame::GCRF()                      // Default estimation frame

/// @brief Orbit Determination solver using least squares
class OrbitDeterminationSolver
{
   public:
    class Analysis
    {
       public:
        /// @brief Constructor
        Analysis(const State& anEstimatedState, const LeastSquaresSolver::Analysis& anAnalysis);

        /// @brief Print analysis
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream) const;

        State estimatedState;  // Matching the frame and expanded coordinates of the provided initial guess state.
        LeastSquaresSolver::Analysis solverAnalysis;
    };

    /// @brief Constructor
    ///
    /// @param anEnvironment Environment, Defaults to Environment::Default()
    /// @param aNumericalSolver Numerical solver, Defaults to NumericalSolver::Default()
    /// @param aSolver Least squares solver, Defaults to LeastSquaresSolver::Default()
    /// @param anEstimationFrameSPtr Estimation frame, Defaults to Frame::GCRF()
    OrbitDeterminationSolver(
        const Environment& anEnvironment = DEFAULT_ENVIRONMENT,
        const NumericalSolver& aNumericalSolver = DEFAULT_NUMERICAL_SOLVER,
        const LeastSquaresSolver& aSolver = DEFAULT_LEAST_SQUARES_SOLVER,
        const Shared<const Frame>& anEstimationFrameSPtr = DEFAULT_ESTIMATION_FRAME
    );

    /// @brief Access environment
    ///
    /// @return Environment
    const Environment& accessEnvironment() const;

    /// @brief Access propagator
    ///
    /// @return Propagator
    const Propagator& accessPropagator() const;

    /// @brief Access solver
    ///
    /// @return Solver
    const LeastSquaresSolver& accessSolver() const;

    /// @brief Access estimation frame
    ///
    /// @return Estimation frame
    const Shared<const Frame>& accessEstimationFrame() const;

    /// @brief Estimate state from observations
    ///
    /// @param anInitialGuessState Initial guess state
    /// @param anObservationStateArray Observations to fit against
    /// @param anEstimationCoordinateSubsets Coordinate subsets to estimate. Defaults to empty array, in which case all
    /// the coordinate subsets from the initial guess state are estimated
    /// @param anInitialGuessSigmas Map of sigmas for initial guess
    /// @param anObservationSigmas Map of sigmas for observations
    ///
    /// @return Analysis
    Analysis estimate(
        const State& anInitialGuessState,
        const Array<State>& anObservationStateArray,
        const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets =
            Array<Shared<const CoordinateSubset>>::Empty(),
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas = DEFAULT_OBSERVATION_SIGMAS
    ) const;

    /// @brief Estimate a Propagated Orbit orbit from observations
    ///
    /// @param anInitialGuessState Initial guess state
    /// @param anObservationStateArray Observations to fit against
    /// @param anEstimationCoordinateSubsets Coordinate subsets to estimate
    /// @param anInitialGuessSigmas Map of sigmas for initial guess
    /// @param anObservationSigmas Map of sigmas for observations
    ///
    /// @return Orbit
    Orbit estimateOrbit(
        const State& anInitialGuessState,
        const Array<State>& anObservationStateArray,
        const Array<Shared<const CoordinateSubset>>& anEstimationCoordinateSubsets =
            Array<Shared<const CoordinateSubset>>::Empty(),
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas = DEFAULT_OBSERVATION_SIGMAS
    ) const;

   private:
    Environment environment_;
    Propagator propagator_;
    LeastSquaresSolver solver_;
    Shared<const Frame> estimationFrameSPtr_;
};

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
