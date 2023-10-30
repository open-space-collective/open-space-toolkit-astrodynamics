/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Real;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;
using ostk::math::obj::MatrixXd;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

using ostk::astro::trajectory::State;

/// @brief                      Finite Difference solver

class FiniteDifferenceSolver
{
   public:
    enum class Type
    {
        Forward,
        Backward,
        Central
    };
    /// @brief                  Constructor
    ///
    /// @code
    ///                         const Real stepPercentage = 1e-3;
    ///
    ///                         FiniteDifferenceSolver finiteDifferenceSolver = {stepPercentage};
    ///
    /// @endcode
    ///
    /// @param                  [in] aType A Finite Difference type.

    FiniteDifferenceSolver(const Type& aType);

    /// @brief                  Output stream operator.
    ///
    /// @param                  [in] anOutputStream An output stream.
    /// @param                  [in] aFiniteDifference A finite difference solver.
    /// @return                 An output stream.

    friend std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifference);

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstantArray An array of instants.
    /// @param                  [in] getStates Callable to generate States at the requested Instants.
    /// @param                  [in] aStepPercentage The step percentage to use for the perturbation. Defaults to 1e-3.
    ///
    /// @return                 The Jacobian

    MatrixXd computeJacobian(
        const State& aState,
        const Array<Instant>& anInstantArray,
        std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
        const Real& aStepPercentage = 1e-3
    );

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstant An instant.
    /// @param                  [in] getState Callable to generate a State at the requested Instant.
    /// @param                  [in] aStepPercentage The step percentage to use for the perturbation. Defaults to 1e-3.
    ///
    /// @return                 The Jacobian

    MatrixXd computeJacobian(
        const State& aState,
        const Instant& anInstant,
        std::function<State(const State&, const Instant&)> getState,
        const Real& aStepPercentage = 1e-3
    );

    /// @brief                  Compute the gradient
    ///
    /// @param                  [in] aState The state to compute the gradient of.
    /// @param                  [in] getState Callable that generates the state given an initial state and target
    /// instant.
    ///
    /// @return                 The gradient

    VectorXd computeGradient(const State& aState, std::function<State(const State&, const Instant&)> getState, const Duration& aStepSize);

    /// @brief                  Print the solver.
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    const Type type_;
};

}  // namespace solvers
}  // namespace astro
}  // namespace ostk

#endif
