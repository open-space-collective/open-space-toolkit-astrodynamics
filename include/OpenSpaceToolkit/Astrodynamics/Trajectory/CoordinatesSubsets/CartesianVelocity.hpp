/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>

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
using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;

/// @brief  Cartesian Velocity.

class CartesianVelocity : public CoordinatesSubset
{
   public:
    static const String DEFAULT_NAME;

    /// @brief              Constructor
    ///
    /// @param              [in] aName a name
    /// @param              [in] aCartesianPositionSPtr the associated Cartesian position

    CartesianVelocity(const String& aName, const Shared<const CartesianPosition>& aCartesianPositionSPtr);

    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker,
        const Shared<const Frame>& toFrame
    ) const override;

    /// @brief              Returns a new instance associated with the given position.
    ///
    /// @param              [in] aCartesianPositionSPtr the associated Cartesian position
    ///
    /// @return             A new shared pointer

    static Shared<const CartesianVelocity> FromPosition(const Shared<const CartesianPosition>& aCartesianPositionSPtr);

    /// @brief              Returns the default three-dimensional instance
    ///
    /// @return             The defaul three-dimensional instance
    static Shared<const CartesianVelocity> ThreeDimensional();

   private:
    static const Shared<const CartesianVelocity> THREE_DIMENSIONAL;
    Shared<const CartesianPosition> cartesianPositionSPtr_;
};

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
