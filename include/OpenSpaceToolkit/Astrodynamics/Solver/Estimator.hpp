/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solver_Estimator__
#define __OpenSpaceToolkit_Astrodynamics_Solver_Estimator__

#include <unordered_map>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

#define DEFAULT_INITIAL_GUESS_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()  // Initial guess sigmas
#define DEFAULT_OBSERVATION_SIGMAS std::unordered_map<CoordinateSubset, VectorXd>()    // Observation sigmas

/// @brief Base class for an estimator
class Estimator
{
   public:
    class Analysis
    {
       public:
        /// @brief Constructor
        Analysis(
            const Real& anRmsError,
            const Array<State>& anEstimatedStatesArray,
            const MatrixXd& anEstimatedCovariance,
            const Array<State>& aComputedObservationsStateArray,
        );

        /// @brief Stream analysis
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief computeResidualStates
        Array<State> computeResidualStates(const Array<State>& anObservationStateArray) const;

        Real rmsError;
        Size observationCount;
        Array<State> estimatedStates;
        MatrixXd estimatedCovariance;
        Array<State> computedObservationStates;
    };

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
    ) const = 0;
};

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk

#endif
