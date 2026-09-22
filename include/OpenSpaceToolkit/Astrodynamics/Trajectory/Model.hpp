/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error/RuntimeError.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

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
using ostk::core::type::String;

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
    /// @brief Error raised when a state is requested at an instant outside of the time interval over which the model is
    /// defined (e.g. a tabulated model).
    ///
    /// @details Models raise the more specific BeforeStartError or AfterEndError, which both derive from this class.
    class OutOfBoundsError : public ostk::core::error::RuntimeError
    {
       public:
        /// @brief Get the instant at which the state was requested.
        ///
        /// @return The instant at which the state was requested.
        Instant getInstant() const;

        /// @brief Get the time interval over which the model is defined.
        ///
        /// @return The time interval over which the model is defined.
        Interval getInterval() const;

       protected:
        /// @brief Constructor.
        ///
        /// @param anInstant The instant at which the state was requested.
        /// @param anInterval The time interval over which the model is defined.
        /// @param aMessage The error message.
        OutOfBoundsError(const Instant& anInstant, const Interval& anInterval, const String& aMessage);

       private:
        Instant instant_;
        Interval interval_;
    };

    /// @brief Error raised when a state is requested at an instant before the start of the time interval over which
    /// the model is defined.
    class BeforeStartError : public OutOfBoundsError
    {
       public:
        /// @brief Constructor.
        ///
        /// @param anInstant The instant at which the state was requested.
        /// @param anInterval The time interval over which the model is defined.
        BeforeStartError(const Instant& anInstant, const Interval& anInterval);
    };

    /// @brief Error raised when a state is requested at an instant after the end of the time interval over which the
    /// model is defined.
    class AfterEndError : public OutOfBoundsError
    {
       public:
        /// @brief Constructor.
        ///
        /// @param anInstant The instant at which the state was requested.
        /// @param anInterval The time interval over which the model is defined.
        AfterEndError(const Instant& anInstant, const Interval& anInterval);
    };

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

    /// @brief Calculate state at a given instant.
    ///
    /// @param anInstant An instant.
    /// @return State at the given instant.
    /// @throw BeforeStartError If the model is only defined over a bounded time interval and the instant is before its
    /// start.
    /// @throw AfterEndError If the model is only defined over a bounded time interval and the instant is after its end.
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
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
