/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Maneuver__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Maneuver__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

/// @brief                      Trajectory maneuver
///                             Dummy Model, to be improved

class Maneuver
{
   public:

    // Add Type or Child Class

    Maneuver(const Instant& anInstant, const Duration& aDuration, const Velocity& aVelocityDelta);

    bool operator==(const Maneuver& aManeuver) const;

    bool operator!=(const Maneuver& aManeuver) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Maneuver& aManeuver);

    bool isDefined() const;

    const Instant& accessInstant() const;

    const Duration& accessDuration() const;

    const Velocity& accessVelocityDelta() const;

    Instant getInstant() const;

    Duration getDuration() const;

    Velocity getVelocityDelta() const;

    // Maneuver inFrame(const Shared<const Frame>& aFrameSPtr) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static Maneuver Undefined();

   private:
    Instant instant_;
    Duration duration_;
    Velocity velocityDelta_;
    // Add concept of Attitude Provider (similar to Orekit/STK)
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
