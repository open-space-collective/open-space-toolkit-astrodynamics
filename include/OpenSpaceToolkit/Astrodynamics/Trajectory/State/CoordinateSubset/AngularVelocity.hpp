/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AngularVelocity__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AngularVelocity__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatesubset
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;

/// @brief  Angular Velocity.

class AngularVelocity : public CoordinateSubset
{
   public:
    /// @brief Constructor
    ///
    /// @code
    ///                  AngularVelocity angularVelocity = {anAttitudeQuaternionSPtr, aName};
    /// @endcode
    ///
    /// @param anAttitudeQuaternionSPtr the associated Attitude Quaternion
    /// @param aName a name
    AngularVelocity(const Shared<const AttitudeQuaternion>& anAttitudeQuaternionSPtr, const String& aName);

    /// @brief Destructor
    ~AngularVelocity();

    /// @brief Transform the coordinate subset from one frame to another
    ///
    /// @param anInstant the reference frame associated to the coordinates
    /// @param aFullCoordinatesVector all coordinates
    /// @param fromFrame the reference frame associated to the coordinates
    /// @param toFrame the reference frame in which the coordinates are to be transformed
    /// @param aCoordinateBrokerSPtr a coordinate broker
    ///
    /// @return The resulting coordinate subset value expressed in the desired reference frame
    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const;

    /// @brief Return the default instance
    ///
    /// @return The default instance
    static Shared<const AngularVelocity> Default();

   private:
    Shared<const AttitudeQuaternion> attitudeQuaternionSPtr_;
};

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
