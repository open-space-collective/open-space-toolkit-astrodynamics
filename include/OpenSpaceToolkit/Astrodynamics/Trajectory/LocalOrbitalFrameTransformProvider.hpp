/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::physics::time::Instant;
using ostk::physics::coord::frame::Provider;
using ostk::physics::coord::frame::Transform;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

/// @brief                      Local orbital frame transform provider

class LocalOrbitalFrameTransformProvider : public Provider
{
   public:
    typedef std::function<Transform(const Instant&, const Position&, const Velocity&)> Generator;

    enum class Type
    {

        Undefined,  ///< Undefined frame
        NED,        ///< North-East-Down (NED) frame
        LVLH,    ///< Local Vertical, Local Horizontal (LVLH) frame (X axis aligned with position, Z axis aligned with
                 ///< orbital momentum)
        VVLH,    ///< Vehicle Velocity, Local Horizontal (VVLH) frame (Z axis aligned with opposite of position, Y axis
                 ///< aligned with opposite of orbital momentum)
        LVLHGD,  ///< Local Vertical, Local Horizontal GeoDetic (LVLHGD) frame
        QSW,     ///< QSW frame (X axis aligned with position, Z axis aligned with orbital momentum)
        TNW,     ///< TNW frame (X axis aligned with velocity, Z axis aligned with orbital momentum)
        VNC  ///< Velocity - Normal - Co-normal (VNC) frame (X axis aligned with velocity, Y axis aligned with orbital
             ///< momentum)
    };

    virtual ~LocalOrbitalFrameTransformProvider() override;

    virtual LocalOrbitalFrameTransformProvider* clone() const override;

    virtual bool isDefined() const override;

    virtual Transform getTransformAt(const Instant& anInstant) const override;

    Transform getTransformAt(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) const;

    LocalOrbitalFrameTransformProvider::Type getType() const;

    static Shared<const LocalOrbitalFrameTransformProvider> Construct(
        const LocalOrbitalFrameTransformProvider::Type& aType
    );

    static String StringFromType(const LocalOrbitalFrameTransformProvider::Type& aType);

   private:
    LocalOrbitalFrameTransformProvider::Type type_;
    Generator generator_;

    LocalOrbitalFrameTransformProvider(
        const LocalOrbitalFrameTransformProvider::Type& aType, const LocalOrbitalFrameTransformProvider::Generator& aGenerator
    );
};


}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
