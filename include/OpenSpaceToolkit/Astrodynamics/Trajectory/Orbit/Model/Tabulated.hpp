/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::type::Integer;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

/// @brief Tabulated orbit model.
///
/// @details Orbit model based on tabulated states with interpolation, extending the trajectory tabulated model with
/// revolution number tracking.
class Tabulated : public virtual trajectory::orbit::Model, public trajectory::model::Tabulated
{
   public:
    /// @brief Construct a tabulated orbit model from an array of states.
    ///
    /// @code{.cpp}
    ///     Array<State> states = { ... };
    ///     Tabulated tabulated = Tabulated(states, 1, Interpolator::Type::Linear);
    /// @endcode
    ///
    /// @param aStateArray An array of states to tabulate.
    /// @param anInitialRevolutionNumber The revolution number at the first state epoch.
    /// @param aType The interpolation type to use between tabulated states.
    Tabulated(
        const Array<State>& aStateArray,
        const Integer& anInitialRevolutionNumber,
        const Interpolator::Type& aType = DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE
    );

    /// @brief Clone this tabulated orbit model.
    ///
    /// @return A pointer to a cloned tabulated orbit model.
    virtual Tabulated* clone() const override;

    /// @brief Check equality with another tabulated orbit model.
    ///
    /// @param aTabulatedModel A tabulated orbit model to compare against.
    /// @return True if the models are equal.
    bool operator==(const Tabulated& aTabulatedModel) const;

    /// @brief Check inequality with another tabulated orbit model.
    ///
    /// @param aTabulatedModel A tabulated orbit model to compare against.
    /// @return True if the models are not equal.
    bool operator!=(const Tabulated& aTabulatedModel) const;

    /// @brief Check if the tabulated orbit model is defined.
    ///
    /// @return True if the model is defined.
    virtual bool isDefined() const override;

    /// @brief Get the epoch of the tabulated orbit model.
    ///
    /// @return The epoch instant, corresponding to the first tabulated state.
    virtual Instant getEpoch() const override;

    /// @brief Get the revolution number at epoch.
    ///
    /// @return The revolution number at epoch.
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Calculate the state at a given instant by interpolating the tabulated states.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = Tabulated(states, 1);
    ///     State state = tabulated.calculateStateAt(Instant::Now());
    /// @endcode
    ///
    /// @param anInstant An instant at which to calculate the state.
    /// @return The interpolated orbital state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Print the tabulated orbit model to an output stream.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator around the output.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator) const override;

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    Integer initialRevolutionNumber_;
};

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
