/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion__

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{
namespace coordinatessubsets
{

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::geometry::d3::transformation::rotation::Quaternion;

using ostk::physics::coord::Transform;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

/// @brief  Attitude Quaternion (XYZS format).

class AttitudeQuaternion : public CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     AttitudeQuaternion attitudeQuaternion = {aName};
    /// @endcode
    ///
    /// @param              [in] aName a name

    AttitudeQuaternion(const String& aName);

    /// @brief              Destructor

    ~AttitudeQuaternion();

    /// @brief              Transform the coordinate subset from one frame to another
    ///
    /// @param              [in] anInstant the reference frame associated to the coordinates
    /// @param              [in] aFullCoordinatesVector all coordinates
    /// @param              [in] fromFrame the reference frame associated to the coordinates
    /// @param              [in] toFrame the reference frame in which the coordinates are to be transformed
    /// @param              [in] aCoordinatesBrokerSPtr a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const override;

    /// @brief              Return a Quaternion from coordinates.
    ///
    /// @param              [in] coordinates coordinates vector
    ///
    /// @return
    static Quaternion coordinatesToQuaternion(const VectorXd& coordinates);

    /// @brief              Return coordinates from a Quaternion.
    ///
    /// @param              [in] quaterion quaternion
    ///
    /// @return
    static VectorXd quaterionToCoordinates(const Quaternion& quaternion);

    /// @brief              Return the default instance
    ///
    /// @return             The default instance

    static Shared<const AttitudeQuaternion> Default();
};

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
