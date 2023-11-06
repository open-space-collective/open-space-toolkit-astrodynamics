/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Real;
using ostk::core::types::Size;
using ostk::core::types::String;
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
    ///                         const Type aType = FiniteDifferenceSolver::Type::Forward;
    ///                         const Real aStepPercentage = 1e-3;
    ///                         const Duration aStepDuration = Duration::Milliseconds(1e-3);
    ///
    ///                         FiniteDifferenceSolver finiteDifferenceSolver = {aType};
    ///
    /// @endcode
    ///
    /// @param                  [in] aType A Finite Difference type.
    /// @param                  [in] aStepPercentage The step percentage to use for computing the STM.
    /// @param                  [in] aStepDuration The step duration to use for computing the gradient.

    FiniteDifferenceSolver(const Type& aType, const Real& aStepPercentage, const Duration& aStepDuration);

    /// @brief                  Output stream operator.
    ///
    /// @param                  [in] anOutputStream An output stream.
    /// @param                  [in] aFiniteDifference A finite difference solver.
    /// @return                 An output stream.

    friend std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifference);

    /// @brief                  Get the Type.
    ///
    /// @return                 The Type.

    Type getType() const;

    /// @brief                  Get the step percentage.
    ///
    /// @return                 The step percentage.

    Real getStepPercentage() const;

    /// @brief                  Get the step duration.
    ///
    /// @return                 The step duration.

    Duration getStepDuration() const;

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstantArray An array of instants.
    /// @param                  [in] generateStateCoordinates Callable to generate coordinates of States at the
    /// requested Instants.
    /// @param                  [in] aCoordinatesDimension The dimension of the coordinates produced by
    /// `generateStateCoordinates`.
    ///
    /// @return                 The Jacobian

    MatrixXd computeJacobian(
        const State& aState,
        const Array<Instant>& anInstantArray,
        const std::function<MatrixXd(const State&, const Array<Instant>&)>& generateStateCoordinates,
        const Size& aCoordinatesDimension
    ) const;

    /// @brief                  Compute the Jacobian by perturbing the coordinates
    ///
    /// @param                  [in] aState A state.
    /// @param                  [in] anInstant An instant.
    /// @param                  [in] generateStateCoordinates Callable to generate coordinates of a State at the
    /// requested Instant.
    /// @param                  [in] aCoordinatesDimension The dimension of the coordinates produced by
    /// `generateStateCoordinates`.
    /// @return                 The Jacobian

    MatrixXd computeJacobian(
        const State& aState,
        const Instant& anInstant,
        const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates,
        const Size& aCoordinatesDimension
    ) const;

    /// @brief                  Compute the gradient.
    ///
    /// @param                  [in] aState The state to compute the gradient of.
    /// @param                  [in] generateStateCoordinates Callable to generate coordinates of a State at the
    /// requested Instant.
    ///
    /// @return                 The gradient.

    VectorXd computeGradient(
        const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
    ) const;

    /// @brief                  Print the solver.
    ///
    /// @param                  [in] anOutputStream An output stream.
    /// @param                  [in] (optional) displayDecorators if true, display decorators.

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Convert a type to string
    ///
    /// @param                  [in] aType A type.
    ///
    /// @return                 The string name of the type.

    static String StringFromType(const Type& aType);

    /// @brief                  Default Finite Difference Solver
    ///
    /// @return                 The default Finite Difference Solver.

    static FiniteDifferenceSolver Default();

   private:
    const Type type_;
    const Real stepPercentage_;
    const Duration stepDuration_;
};

}  // namespace solvers
}  // namespace astro
}  // namespace ostk

#endif
