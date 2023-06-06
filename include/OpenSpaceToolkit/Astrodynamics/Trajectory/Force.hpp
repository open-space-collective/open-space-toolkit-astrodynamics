/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Force__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Force__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;

/// @brief                      Force (abstract)

class Force
{
   public:

    /// @brief              Constructor (pure virtual)

    Force();

    /// @brief              Destructor (pure virtual)

    virtual ~Force() = 0;

    /// @brief              Clone dynamics (pure virtual)
    ///
    /// @return             Pointer to cloned force

    virtual Force* clone() const = 0;

    // virtual bool operator==(const Force& aForce) const = 0;

    // virtual bool operator!=(const Force& aForce) const = 0;

    // friend std::ostream& operator<<(std::ostream& anOutputStream, const Force& aForce);

    // virtual bool isDefined() const = 0;

    virtual Vector3d getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const = 0;

    /// @brief              Returns true if model can be converted to type
    ///
    /// @return             True if model can be converted to type

    // template <class Type>
    // bool is() const
    // {
    //     return dynamic_cast<const Type*>(this) != nullptr;
    // }

    /// @brief              Access model as its underlying type
    ///
    /// @return             Reference to underlying type

    // template <class Type>
    // const Type& as() const
    // {
    //     const Type* modelPtr = dynamic_cast<const Type*>(this);

    //     if (modelPtr == nullptr)
    //     {
    //         throw ostk::core::error::RuntimeError("Cannot convert model to underlying type.");
    //     }

    //     return *modelPtr;
    // }

    // virtual State calculateStateAt(const Instant& anInstant) const = 0;

    // virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const;

    // virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
