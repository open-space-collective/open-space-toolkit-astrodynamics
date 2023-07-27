/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition__

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

using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

/// @brief  Cartesian Position.

class CartesianPosition : public CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @param              [in] aName a name

    CartesianPosition(const String& aName);

    /// @brief              Destructor

    ~CartesianPosition();

    /// @brief              Adds two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinatesVector first set of all coordinates
    /// @param              [in] anotherFullCoordinatesVector second set of all coordinates
    /// @param              [in] aFrame the reference frame in which the coordinates are resolved
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (aSubset + anotherSubset)

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override;

    /// @brief              Subtracts two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinatesVector first set of all coordinates
    /// @param              [in] anotherFullCoordinatesVector second set of all coordinates
    /// @param              [in] aFrame the reference frame associated to the coordinates
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (aSubset - anotherSubset)

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override;

    /// @brief              Transforms the coordinate subset from one frame to another
    ///
    /// @param              [in] anInstant the reference frame associated to the coordinates
    /// @param              [in] aFullCoordinatesVector all coordinates
    /// @param              [in] fromFrame the reference frame associated to the coordinates
    /// @param              [in] toFrame the reference frame in which the coordinates are to be transformed
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override;

    /// @brief              Return the default instance
    ///
    /// @return             The default instance

    static Shared<const CartesianPosition> Default();

   private:
    static const Shared<const CartesianPosition> DEFAULT;
};

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
