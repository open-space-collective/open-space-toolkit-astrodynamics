/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AttitudeQuaternion.hpp>

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

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::time::Instant;
using ostk::physics::coordinate::Frame;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::AttitudeQuaternion;

/// @brief  Angular Velocity.

class AngularVelocity : public CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     AngularVelocity angularVelocity = {anAttitudeQuaternionSPtr, aName};
    /// @endcode
    ///
    /// @param              [in] anAttitudeQuaternionSPtr the associated Attitude Quaternion
    /// @param              [in] aName a name

    AngularVelocity(const Shared<const AttitudeQuaternion>& anAttitudeQuaternionSPtr, const String& aName);

    /// @brief              Destructor

    ~AngularVelocity();

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
    ) const;

    /// @brief              Return the default instance
    ///
    /// @return             The default instance

    static Shared<const AngularVelocity> Default();

   private:
    Shared<const AttitudeQuaternion> attitudeQuaternionSPtr_;
};

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
