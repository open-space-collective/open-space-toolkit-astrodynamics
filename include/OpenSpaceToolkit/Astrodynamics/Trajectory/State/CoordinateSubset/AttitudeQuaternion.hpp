/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion__

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatessubset
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

using ostk::physics::coordinate::Transform;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;

/// @brief  Attitude Quaternion (XYZS format).

class AttitudeQuaternion : public CoordinateSubset
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
    /// @param              [in] aCoordinateBrokerSPtr a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
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

}  // namespace coordinatessubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
