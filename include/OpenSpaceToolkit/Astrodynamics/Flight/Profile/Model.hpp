/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace profile
{

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

/// @brief Profile model (abstract).
///
/// @details Base class for all flight profile models. Provides the interface for calculating states
/// and body-frame axes at given instants. Concrete implementations include Tabulated and Transform models.
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
    virtual bool operator!=(const Model& aModel) const;

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
    virtual State calculateStateAt(const Instant& anInstant) const = 0;

    /// @brief Calculate states at given instants.
    ///
    /// @param anInstantArray An array of instants.
    /// @return Array of states.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Get axes at a given instant.
    ///
    /// @param anInstant An instant.
    /// @return Axes at the given instant.
    virtual Axes getAxesAt(const Instant& anInstant) const = 0;

    /// @brief Construct a body frame.
    ///
    /// @param aFrameName A frame name.
    /// @return Shared pointer to body frame.
    virtual Shared<const Frame> constructBodyFrame(const String& aFrameName) const = 0;

    /// @brief Print model.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorator.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
