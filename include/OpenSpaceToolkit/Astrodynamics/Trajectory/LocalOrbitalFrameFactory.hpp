/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory__

#include <memory>

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::Provider;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Vector3d;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

/// @brief A local orbital frame factory
///
/// @note                       Allows to generate Frames based on instant, position, velocity.
class LocalOrbitalFrameFactory
{
   public:
    typedef std::function<Transform(const State&)> TransformGenerator;
    /// @brief Generate a frame shared pointer based on current state input
    ///
    /// @param aState A State
    ///
    /// @return A shared pointer to the frame created
    Shared<const Frame> generateFrame(const State& aState) const;

    /// @brief Check if local orbital frame factory is defined
    ///
    /// @return True if local orbital frame factory is defined
    bool isDefined() const;

    /// @brief Accessor for the parent frame.
    ///
    /// @return The parent frame
    const Shared<const Frame>& accessParentFrame() const;

    /// @brief Get type
    ///
    /// @return Type
    LocalOrbitalFrameTransformProvider::Type getProviderType() const;

    /// @brief NED Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> NED(const Shared<const Frame>& aParentFrame);

    /// @brief LVLH Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> LVLH(const Shared<const Frame>& aParentFrame);

    /// @brief VVLH Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> VVLH(const Shared<const Frame>& aParentFrame);

    /// @brief QSW Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> QSW(const Shared<const Frame>& aParentFrame);

    /// @brief TNW Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> TNW(const Shared<const Frame>& aParentFrame);

    /// @brief VNC Constructor
    ///
    /// @param aParentFrame A parent frame
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> VNC(const Shared<const Frame>& aParentFrame);

    /// @brief Undefined local orbital frame factory constructor
    ///
    /// @return Undefined local orbital frame factory shared pointer
    static Shared<const LocalOrbitalFrameFactory> Undefined();

    /// @brief Construct function
    ///
    /// @param aType The type of local orbital frame transform provider
    /// @param aParentFrame The parent frame of the local orbital frame factory
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> Construct(
        const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
    );

    /// @brief Construct custom frame
    ///
    /// @param aTransformGenerator A function to generate the transform
    /// @param aParentFrame The parent frame of the local orbital frame factory
    ///
    /// @return A shared pointer to a LocalOrbitalFrameFactory
    static Shared<const LocalOrbitalFrameFactory> Construct(
        const TransformGenerator& aTransformGenerator, const Shared<const Frame>& aParentFrame
    );

   private:
    LocalOrbitalFrameTransformProvider::Type type_;
    Shared<const Frame> parentFrameSPtr_;
    TransformGenerator transformGenerator_;

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  LocalOrbitalFrameFactory localOrbitalFrameFactory = {aType, aParentFrame, aTransformGenerator};
    /// @endcode
    ///
    /// @param aType The type of local orbital frame transform provider
    /// @param aParentFrame The parent frame of the local orbital frame factory
    /// @param aTransformGenerator A function to generate the transform
    LocalOrbitalFrameFactory(
        const LocalOrbitalFrameTransformProvider::Type& aType,
        const Shared<const Frame>& aParentFrame,
        const TransformGenerator& aTransformGenerator
    );

    /// @brief Generate a frame name based on current state
    ///
    /// @param aState A state
    ///
    /// @return A frame name
    String generateFrameName(const State& aState) const;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
