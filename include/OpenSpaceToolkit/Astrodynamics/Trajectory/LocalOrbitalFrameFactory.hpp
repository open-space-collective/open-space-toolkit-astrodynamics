/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__

#include <memory>

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

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

/// @brief                      A local orbital frame factory
///
/// @note                       Allows to generate Frames based on instant, position, velocity.

class LocalOrbitalFrameFactory
{
   public:
    /// @brief                  Generate a frame shared pointer based on current state input
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPosition A position vector
    /// @param                  [in] aVelocity A velocity vector
    ///
    /// @return                 A shared pointer to the frame created

    Shared<const Frame> generateFrame(const Instant& anInstant, const Vector3d& aPosition, const Vector3d& aVelocity)
        const;

    /// @brief                  Check if local orbital frame factory is defined
    ///
    /// @return                 True if local orbital frame factory is defined

    bool isDefined() const;

    /// @brief                  Get type
    ///
    /// @return                 Type

    LocalOrbitalFrameTransformProvider::Type getProviderType() const;

    /// @brief                  NED Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> NED(const Shared<const Frame>& aParentFrame);

    /// @brief                  LVLH Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> LVLH(const Shared<const Frame>& aParentFrame);

    /// @brief                  VVLH Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> VVLH(const Shared<const Frame>& aParentFrame);

    /// @brief                  QSW Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> QSW(const Shared<const Frame>& aParentFrame);

    /// @brief                  TNW Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> TNW(const Shared<const Frame>& aParentFrame);

    /// @brief                  VNC Constructor
    ///
    /// @param                  [in] aParentFrame A parent frame
    ///
    /// @return                 A shared pointer to a LocalOrbitalFrameFactory

    static Shared<const LocalOrbitalFrameFactory> VNC(const Shared<const Frame>& aParentFrame);

    /// @brief                  Undefined local orbital frame factory constructor
    ///
    /// @return                 Undefined local orbital frame factory shared pointer

    static Shared<const LocalOrbitalFrameFactory> Undefined();

    /// @brief                  Construct function
    ///
    /// @param                  [in] aType The type of local orbital frame transform provider
    /// @param                  [in] aParentFrame The parent frame of the local orbital frame factory

    static Shared<const LocalOrbitalFrameFactory> Construct(
        const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
    );

   private:
    LocalOrbitalFrameTransformProvider::Type type_;
    Shared<const Frame> parentFrameSPtr_;

    /// @brief                  Constructor
    ///
    /// @code
    ///                         LocalOrbitalFrameFactory localOrbitalFrameFactory = {aType, aParentFrame};
    /// @endcode
    ///
    /// @param                  [in] aType The type of local orbital frame transform provider
    /// @param                  [in] aParentFrame The parent frame of the local orbital frame factory

    LocalOrbitalFrameFactory(
        const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
    );

    /// @brief                  Generate a frame name based on current state
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPosition A position vector
    /// @param                  [in] aVelocity A velocity vector
    ///
    /// @return                 A frame name

    String generateFrameName(const Instant& anInstant, const Vector3d& aPosition, const Vector3d& aVelocity) const;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
