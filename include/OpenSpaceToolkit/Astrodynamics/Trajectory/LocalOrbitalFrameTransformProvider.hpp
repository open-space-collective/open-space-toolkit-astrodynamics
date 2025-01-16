/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider__

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::frame::Provider;
using ostk::physics::coordinate::frame::Transform;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Vector3d;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

/// @brief Local orbital frame transform provider, frame provider
///
/// @note  Generates a specific transform based on instant, position, velocity and a LOF type.
class LocalOrbitalFrameTransformProvider : public Provider
{
   public:
    enum class Type
    {
        Undefined,  ///< Undefined frame
        NED,        ///< North-East-Down (NED) frame
        LVLH,    ///< Local Vertical, Local Horizontal (LVLH) frame (X axis aligned with position, Z axis aligned with
                 ///< orbital momentum)
        VVLH,    ///< Vehicle Velocity, Local Horizontal (VVLH) frame (Y axis aligned with opposite of orbital momentum,
                 ///< Z axis aligned with opposite of position)
        LVLHGD,  ///< Local Vertical, Local Horizontal GeoDetic (LVLHGD) frame
        QSW,     ///< QSW frame (X axis aligned with position, Z axis aligned with orbital momentum)
        TNW,     ///< Tangent-Normal-Wideband (TNW) frame (X axis aligned with velocity, Z axis aligned with orbital
                 ///< momentum)
        VNC,     ///< Velocity-Normal-Co-normal (VNC) frame (X axis aligned with velocity, Y axis aligned with orbital
                 ///< momentum)
        Custom   ///< Custom frame
    };

    /// @brief Constructor
    ///
    /// @param aTransform A transform
    ///
    /// @return A local orbital frame transform provider
    LocalOrbitalFrameTransformProvider(const Transform& aTransform);

    /// @brief Destructor
    virtual ~LocalOrbitalFrameTransformProvider() override;

    /// @brief Clone
    ///
    /// @return Pointer to local orbital frame transform provider
    virtual LocalOrbitalFrameTransformProvider* clone() const override;

    /// @brief Check if local orbital frame transform provider is defined
    ///
    /// @return True if local orbital frame transform provider is defined
    virtual bool isDefined() const override;

    /// @brief Get the transform of the local orbital frame transform provider at a given instant
    ///
    /// @param anInstant An instant
    ///
    /// @return The transform from the provider at provided instant
    virtual Transform getTransformAt(const Instant& anInstant) const override;

    /// @brief Construct a local orbital frame transform provider shared pointer for the provided type
    ///
    /// @param aType A local orbital frame provider type
    /// @param aState A state
    ///
    /// @return The shared pointer to the local orbital frame transform provider constructed
    static Shared<const LocalOrbitalFrameTransformProvider> Construct(
        const LocalOrbitalFrameTransformProvider::Type& aType, const State& aState
    );

    /// @brief Get the transform generator function for a given type
    ///
    /// @param aType A local orbital frame provider type
    ///
    /// @return The transform generator function
    static std::function<Transform(const State&)> GetTransformGenerator(
        const LocalOrbitalFrameTransformProvider::Type& aType
    );

    /// @brief Convert local orbital frame transform provider type to string
    ///
    /// @param aType A local orbital frame provider type
    ///
    /// @return The string corresponding to the local orbital frame transform provider type
    static String StringFromType(const LocalOrbitalFrameTransformProvider::Type& aType);

   private:
    Transform transform_;

    /// @brief Generate a transform based on current state and LOF type
    ///
    /// @param aType A local orbital frame provider type
    /// @param aState
    ///
    /// @return The transform generated
    static Transform GenerateTransform(const LocalOrbitalFrameTransformProvider::Type& aType, const State& aState);
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
