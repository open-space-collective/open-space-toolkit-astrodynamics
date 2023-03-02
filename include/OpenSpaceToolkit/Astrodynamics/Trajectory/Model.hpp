////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::ctnr::Array;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Trajectory model (abstract)

class Model
{
   public:
    Model();

    virtual ~Model() = 0;

    virtual Model* clone() const = 0;

    virtual bool operator==(const Model& aModel) const = 0;

    virtual bool operator!=(const Model& aModel) const = 0;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Model& aModel);

    virtual bool isDefined() const = 0;

    /// @brief              Returns true if model can be converted to type
    ///
    /// @return             True if model can be converted to type

    template <class Type>
    bool is() const
    {
        return dynamic_cast<const Type*>(this) != nullptr;
    }

    /// @brief              Access model as its underlying type
    ///
    /// @return             Reference to underlying type

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

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
