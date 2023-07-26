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
    ///                     CartesianVelocity velocity = {aCartesianPositionSPtr, aName};
    /// @endcode
    ///
    /// @param              [in] aCartesianPositionSPtr the associated Cartesian position
    /// @param              [in] aName a name

    CartesianVelocity(
        const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName = DEFAULT_NAME
    );

    /// @brief              Destructor

    ~CartesianVelocity();

    /// @brief              Adds two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinates first set of all coordinates
    /// @param              [in] anotherFullCoordinates second set of all coordinates
    /// @param              [in] aFrame the reference frame in which the coordinates are resolved
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (aSubset + anotherSubset)

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinates,
        const VectorXd& anotherFullCoordinates,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    /// @brief              Subtracts two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinates first set of all coordinates
    /// @param              [in] anotherFullCoordinates second set of all coordinates
    /// @param              [in] aFrame the reference frame associated to the coordinates
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (aSubset - anotherSubset)

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinates,
        const VectorXd& anotherFullCoordinates,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    /// @brief              Transforms the coordinate subset from one frame to another
    ///
    /// @param              [in] anInstant the reference frame associated to the coordinates
    /// @param              [in] allCoordinates all coordinates
    /// @param              [in] fromFrame the reference frame associated to the coordinates
    /// @param              [in] toFrame the reference frame in which the coordinates are to be transformed
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    static String DEFAULT_NAME;

   private:
    Shared<const CartesianPosition> cartesianPositionSPtr_;
};

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
