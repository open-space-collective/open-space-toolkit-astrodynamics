/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{

using ostk::core::type::Integer;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

/// @brief Abstract base class for orbit models.
class Model : public virtual ostk::astrodynamics::trajectory::Model
{
   public:
    /// @brief Default constructor.
    Model();

    /// @brief Destructor.
    virtual ~Model() = 0;

    /// @brief Clone the orbit model.
    ///
    /// @return A pointer to the cloned orbit model.
    virtual Model* clone() const = 0;

    /// @brief Check if the orbit model is defined.
    ///
    /// @return True if the orbit model is defined, false otherwise.
    virtual bool isDefined() const = 0;

    /// @brief Get the epoch of the orbit model.
    ///
    /// @return The epoch instant.
    virtual Instant getEpoch() const = 0;

    /// @brief Get the revolution number at the epoch.
    ///
    /// @return The revolution number at epoch.
    virtual Integer getRevolutionNumberAtEpoch() const = 0;

    /// @brief Calculate the state at a given instant.
    ///
    /// @param anInstant An instant at which to calculate the state.
    /// @return The state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const = 0;

    /// @brief Calculate the revolution number at a given instant.
    ///
    /// @code{.cpp}
    ///     Integer revolutionNumber = model.calculateRevolutionNumberAt(instant) ;
    /// @endcode
    ///
    /// @param anInstant An instant at which to calculate the revolution number.
    /// @return The revolution number at the given instant.
    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const;

    /// @brief Print the orbit model to an output stream.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator around the output.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
