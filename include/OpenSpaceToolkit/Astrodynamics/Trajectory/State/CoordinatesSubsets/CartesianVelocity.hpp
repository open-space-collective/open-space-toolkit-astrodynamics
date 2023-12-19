/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianVelocity__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianVelocity__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>

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

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;

/// @brief  Cartesian Velocity.

class CartesianVelocity : public CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     CartesianVelocity cartesianVelocity = {aCartesianPositionSPtr, aName};
    /// @endcode
    ///
    /// @param              [in] aCartesianPositionSPtr the associated Cartesian Position
    /// @param              [in] aName a name

    CartesianVelocity(const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName);

    /// @brief              Destructor

    ~CartesianVelocity();

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

    static Shared<const CartesianVelocity> Default();

   private:
    Shared<const CartesianPosition> cartesianPositionSPtr_;
};

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
