/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_State__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_State__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{

using ostk::core::types::Shared;

using ostk::math::geometry::d3::trf::rot::Quaternion;
using ostk::math::object::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

/// @brief                      Spacecraft flight profile state

class State
{
   public:
    State(
        const Instant& anInstant,
        const Position& aPosition,
        const Velocity& aVelocity,
        const Quaternion& anAttitude,
        const Vector3d& anAngularVelocity,
        const Shared<const Frame>& aReferenceFrame
    );

    bool operator==(const State& aState) const;

    bool operator!=(const State& aState) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const State& aState);

    bool isDefined() const;

    const Instant& accessInstant() const;

    const Position& accessPosition() const;

    const Velocity& accessVelocity() const;

    const Quaternion& accessAttitude() const;

    const Vector3d& accessAngularVelocity() const;

    Instant getInstant() const;

    Position getPosition() const;

    Velocity getVelocity() const;

    Quaternion getAttitude() const;

    Vector3d getAngularVelocity() const;

    Shared<const Frame> getFrame() const;

    State inFrame(const Shared<const Frame>& aFrameSPtr) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static State Undefined();

   private:
    Instant instant_;
    Position position_;
    Velocity velocity_;
    Quaternion attitude_;
    Vector3d angularVelocity_;
    Shared<const Frame> frameSPtr_;
};

}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
