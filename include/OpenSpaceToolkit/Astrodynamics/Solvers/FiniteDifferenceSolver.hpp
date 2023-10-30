/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solvers__
#define __OpenSpaceToolkit_Astrodynamics_Solvers__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Real;
using ostk::core::types::Size;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;
using ostk::math::obj::MatrixXd;

using ostk::physics::time::Instant;

/// @brief                      Finite Difference solver

class FiniteDifference
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         const Real stepPercentage = 1e-3;
    ///
    ///                         FiniteDifference finiteDifference = {stepPercentage};
    ///
    /// @endcode
    ///
    /// @param                  [in] stepPercentage A step size percentage. Defaults to 1e-3

    FiniteDifference(const Real& stepPercentage = 1e-3);

    /// @brief                  Output stream operator.
    ///
    /// @param                  [in] anOutputStream An output stream.
    /// @param                  [in] aFiniteDifference A finite difference solver.
    /// @return                 An output stream.

    friend std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifference& aFiniteDifference);

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstantArray An array of instants.
    /// @param                  [in] getStates Callable to generate States at the requested instant array
    ///
    /// @return                 The gradient

    MatrixXd computeJacobian(
        const State& aState,
        const Array<Instant>& anInstantArray,
        std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
    );

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstant An instant.
    /// @param                  [in] getState Callable to generate a State at the requested Instant
    ///
    /// @return                 The gradient

    MatrixXd computeJacobian(
        const State& aState, const Instant& anInstant, std::function<State(const State&, const Instant&)> getState,
    );

    /// @brief                  Compute the gradient
    ///
    /// @param                  [in] aState The state to compute the gradient of.
    /// @param                  [in] getStates Callable that generates the state given an initial state and target
    /// instant.
    ///
    /// @return                 The gradient

    VectorXd computeGradient(
        const State& aState, std::function<Array<VectorXd>(const VectorXd&, const Array<Instant>&)> getStates,
    );

    /// @brief                  Print the solver.
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    const Real stepPercentage_;
};

}  // namespace solvers
}  // namespace astro
}  // namespace ostk

#endif
