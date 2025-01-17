/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

/// @brief Finite Difference solver
class FiniteDifferenceSolver
{
   public:
    enum class Type
    {
        Forward,
        Backward,
        Central
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  const Type aType = FiniteDifferenceSolver::Type::Forward;
    ///                  const Real aStepPercentage = 1e-3;
    ///                  const Duration aStepDuration = Duration::Milliseconds(1e-3);
    ///
    ///                  FiniteDifferenceSolver finiteDifferenceSolver = {aType};
    ///
    /// @endcode
    ///
    /// @param aType A Finite Difference type.
    /// @param aStepPercentage The step percentage to use for computing the STM.
    /// @param aStepDuration The step duration to use for computing the gradient.
    FiniteDifferenceSolver(const Type& aType, const Real& aStepPercentage, const Duration& aStepDuration);

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aFiniteDifferenceSolver A finite difference solver.
    /// @return An output stream.
    friend std::ostream& operator<<(
        std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifferenceSolver
    );

    /// @brief Get the Type.
    ///
    /// @return The Type.
    Type getType() const;

    /// @brief Get the step percentage.
    ///
    /// @return The step percentage.
    Real getStepPercentage() const;

    /// @brief Get the step duration.
    ///
    /// @return The step duration.
    Duration getStepDuration() const;

    /// @brief Compute the State Transition Matrices (STMs) by perturbing the coordinates
    ///
    /// @param aState A state from which to compute the STMs.
    /// @param anInstantArray An array of instants at which to compute the STMs.
    /// @param generateStatesCoordinates Callable to generate coordinates of States at the
    /// requested Instants. The callable should return a MatrixXd where each column is the coordinates at an instant.
    ///
    /// @return The State Transition Matrices (STMs)
    Array<MatrixXd> computeStateTransitionMatrix(
        const State& aState,
        const Array<Instant>& anInstantArray,
        const std::function<MatrixXd(const State&, const Array<Instant>&)>& generateStatesCoordinates
    ) const;

    /// @brief Compute the State Transition Matrix (STM) by perturbing the coordinates
    ///
    /// @param aState A state.
    /// @param anInstant An instant.
    /// @param generateStateCoordinates Callable to generate coordinates of a State at the
    /// requested Instant. Must be a column vector.
    /// @return The State Transition Matrix (STM)
    MatrixXd computeStateTransitionMatrix(
        const State& aState,
        const Instant& anInstant,
        const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
    ) const;

    /// @brief Compute the gradient.
    ///
    /// @param aState The state to compute the gradient of.
    /// @param generateStateCoordinates Callable to generate coordinates of a State at the
    /// requested Instant.
    ///
    /// @return The gradient.
    VectorXd computeGradient(
        const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
    ) const;

    /// @brief Compute the Jacobian.
    ///
    /// @param aState The state to compute the Jacobian of.
    /// @param generateStateCoordinates Callable to generate coordinates of the State.
    ///
    /// @return The Jacobian.
    MatrixXd computeJacobian(
        const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
    ) const;

    /// @brief Print the solver.
    ///
    /// @param anOutputStream An output stream.
    /// @param (optional) displayDecorators if true, display decorators.
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Convert a type to string
    ///
    /// @param aType A type.
    ///
    /// @return The string name of the type.
    static String StringFromType(const Type& aType);

    /// @brief Default Finite Difference Solver
    ///
    /// @return The default Finite Difference Solver.
    static FiniteDifferenceSolver Default();

   private:
    const Type type_;
    const Real stepPercentage_;
    const Duration stepDuration_;
};

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk

#endif
