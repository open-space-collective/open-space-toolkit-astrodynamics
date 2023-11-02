/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{
namespace thruster
{

using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;

using ostk::physics::time::Instant;

/// @brief                      An interface for a Guidance Law that can computes an acceleration contribution. To be
/// used in conjunction with a Thruster class to propagate a satellite to a target orbit.

class GuidanceLaw
{
   public:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aName A name

    GuidanceLaw(const String& aName);

    /// @brief                  Destructor

    virtual ~GuidanceLaw();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << GuidanceLaw(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aGuidanceLaw A guidance Law
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw);

    /// @brief                  Get name
    ///
    /// @return                 The name

    String getName() const;

    /// @brief                  Print guidance law
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Compute the acceleration at the provided coordinates based on logic specific to the
    /// guidance law
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPositionCoordinates The position coordinates
    /// @param                  [in] aVelocityCoordinates The velocity coordinates
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    /// @param                  [in] outputFrameSPtr The frame in which the acceleration is expressed
    ///
    /// @return                 The acceleration at the provided coordinates

    virtual Vector3d computeAcceleration(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const = 0;

   protected:
    const String name_;
};

}  // namespace thruster
}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
