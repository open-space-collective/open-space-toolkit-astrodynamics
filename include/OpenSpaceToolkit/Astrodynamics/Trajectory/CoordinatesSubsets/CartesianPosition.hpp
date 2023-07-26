/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition__

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesSubset;
using ostk::astro::trajectory::CoordinatesBroker;

/// @brief  Cartesian Position.

class CartesianPosition : public CoordinatesSubset
{
   public:
    static const String DEFAULT_NAME;

    /// @brief              Constructor
    ///
    /// @param              [in] aName a name
    /// @param              [in] aSize a size [1, 3]

    CartesianPosition(const String& aName, const Size& aSize);

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    /// @brief              Returns the default three-dimensional instance
    ///
    /// @return             The defaul three-dimensional instance

    static Shared<const CartesianPosition> ThreeDimensional();

   private:
    static const Shared<const CartesianPosition> THREE_DIMENSIONAL;
};

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
