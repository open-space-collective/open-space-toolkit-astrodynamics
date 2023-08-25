/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__

#include <memory>

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::physics::coord::frame::Provider;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Vector3d;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;

/// @brief                      Local orbital reference frame
///
/// @note                       Implementation heavily inspired by (the great!)
/// https://www.orekit.org/static/apidocs/org/orekit/frames/LocalOrbitalFrameFactory.html

class LocalOrbitalFrameFactory
{
   public:
    Shared<const Frame> generateFrame(
        const Instant& anInstant,
        const Vector3d& aPosition,
        const Vector3d& aVelocity
    );

    static Shared<const LocalOrbitalFrameFactory> Construct(
        const LocalOrbitalFrameTransformProvider::Type& aType,
        const Shared<const Frame>& aParentFrame);

    static Shared<const LocalOrbitalFrameFactory> VNC(const Shared<const Frame>& aParentFrame);

   private:
    LocalOrbitalFrameTransformProvider::Type type_;
    Shared<const Frame> parentFrame_;

    LocalOrbitalFrameFactory(
        const LocalOrbitalFrameTransformProvider::Type& aType,
        const Shared<const Frame>& aParentFrame
    );

    String generateFrameName(
        const Instant& anInstant,
        const Vector3d& aPosition,
        const Vector3d& aVelocity);
};

}  // namespace coord
}  // namespace physics
}  // namespace ostk

#endif
