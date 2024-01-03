/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::coord::Axes;
using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::flight::profile::State;

/// @brief Profile model (abstract)
class Model
{
   public:
    Model();

    virtual ~Model() = 0;

    virtual Model* clone() const = 0;

    virtual bool operator==(const Model& aModel) const = 0;

    virtual bool operator!=(const Model& aModel) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Model& aModel);

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

    virtual State calculateStateAt(const Instant& anInstant) const = 0;

    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const;

    virtual Axes getAxesAt(const Instant& anInstant) const = 0;

    virtual Shared<const Frame> getBodyFrame(const String& aFrameName) const = 0;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
