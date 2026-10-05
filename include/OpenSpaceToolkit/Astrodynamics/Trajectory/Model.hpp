/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__

#include <optional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;

using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::State;

/// @brief Trajectory model (abstract).
///
/// @details Base class for all trajectory models. Provides the interface for calculating states
/// at given instants. Concrete implementations include Tabulated, Propagated, SGP4, and Kepler models.
class Model
{
   public:
    /// @brief Default constructor.
    Model();

    /// @brief Destructor (pure virtual).
    virtual ~Model() = 0;

    /// @brief Clone the model.
    ///
    /// @return Pointer to the cloned model.
    virtual Model* clone() const = 0;

    /// @brief Equal to operator.
    ///
    /// @param aModel Another model.
    /// @return True if models are equal.
    virtual bool operator==(const Model& aModel) const = 0;

    /// @brief Not equal to operator.
    ///
    /// @param aModel Another model.
    /// @return True if models are not equal.
    virtual bool operator!=(const Model& aModel) const = 0;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aModel A model.
    /// @return A reference to output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Model& aModel);

    /// @brief Check if model is defined.
    ///
    /// @return True if model is defined.
    virtual bool isDefined() const = 0;

    /// @brief Returns true if model can be converted to type
    ///
    /// @return True if model can be converted to type
    template <class Type>
    bool is() const
    {
        return dynamic_cast<const Type*>(this) != nullptr;
    }

    /// @brief Access model as its underlying type
    ///
    /// @return Reference to underlying type
    template <class Type>
    const Type& as() const
    {
        const Type* modelPtr = dynamic_cast<const Type*>(this);

        if (modelPtr == nullptr)
        {
            throw ostk::core::error::RuntimeError("Cannot convert model to underlying type.");
        }

        return *modelPtr;
    }

    /// @brief Get the time interval over which the model is defined.
    ///
    /// @details Models that can only be evaluated over a bounded time interval (e.g. tabulated models) provide it, and
    /// throw when evaluated outside of it. Other models can be evaluated at any instant.
    ///
    /// @return The time interval over which the model is defined, or std::nullopt if the model is not bounded in time.
    std::optional<Interval> getValidityInterval() const;

    /// @brief Calculate state at a given instant.
    ///
    /// @param anInstant An instant.
    /// @return State at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const = 0;

    /// @brief Calculate states at given instants.
    ///
    /// @param anInstantArray An array of instants.
    /// @return Array of states.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Print model.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorator.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;

   protected:
    /// @brief Set the time interval over which the model is defined.
    ///
    /// @details Called by models that can only be evaluated over a bounded time interval, typically from their
    /// constructor. Being a virtual base, the model cannot receive the interval through its constructor, as it is
    /// constructed by the most derived class.
    ///
    /// @param anInterval The time interval over which the model is defined, or std::nullopt if the model is not
    /// bounded in time.
    void setValidityInterval(const std::optional<Interval>& anInterval);

   private:
    std::optional<Interval> validityInterval_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
