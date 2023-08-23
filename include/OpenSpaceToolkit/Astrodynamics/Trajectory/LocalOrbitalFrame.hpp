/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrame__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrame__

#include <memory>

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
// #include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
// #include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalProvider.hpp>

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

using ostk::astro::trajectory::LOFProvider;

/// @brief                      Local orbital reference frame
///
/// @note                       Implementation heavily inspired by (the great!)
/// https://www.orekit.org/static/apidocs/org/orekit/frames/LocalOrbitalFrame.html

class LocalOrbitalFrame : public Frame
{
   public:
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

    // Shared<const LocalOrbitalFrame> getOrbitalFrame(const LocalOrbitalFrame::Type& aType) const;

    /// @brief              Constructor
    ///
    /// @param              [in] aName A frame name
    /// @param              [in] aParentFrame A shared pointer to the parent frame
    /// @param              [in] aProvider A shared pointer to the transform provider

    static Shared<const LocalOrbitalFrame> Construct(
        const String& aName,
        const Shared<const Frame>& aParentFrame,
        const Shared<const LOFProvider>& aProvider
    );

    /// @brief              Constructs an undefined local orbital frame
    ///
    ///                     Undefined local orbital frame

    static Shared<const LocalOrbitalFrame> Undefined();

    /// @brief              Constructs a VNC local orbital frame
    ///
    /// @return             VNC local orbital frame

    static Shared<const LocalOrbitalFrame> VNC();

    static String StringFromType(const LocalOrbitalFrame::Type& aType);

   protected:
    LocalOrbitalFrame(
        const String& aName,
        const Shared<const Frame>& aParentFrame,
        const Shared<const LOFProvider>& aProvider
    );

    // LocalOrbitalFrame(const LocalOrbitalFrame& aLocalOrbitalFrame) = default;

    // LocalOrbitalFrame& operator=(const LocalOrbitalFrame& aLocalOrbitalFrame) = default;

    private:

        static Shared<const LocalOrbitalFrame> Emplace(
            const String& aName,
            const Shared<const Frame>& aParentFrame,
            const Shared<const LOFProvider>& aProvider
        );

     static String generateFrameName(const LocalOrbitalFrame::Type& aType);

};

}  // namespace coord
}  // namespace physics
}  // namespace ostk

#endif
