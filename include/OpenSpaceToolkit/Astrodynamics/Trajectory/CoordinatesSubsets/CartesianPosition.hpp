/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

using ostk::astro::trajectory::CoordinatesSubset;
using ostk::astro::trajectory::CoordinatesBroker;


/// @brief  Three-dimensional Cartesian Position.

class CartesianPosition : public CoordinatesSubset
{
   public:
    /// @brief              Constructor

    CartesianPosition();

    virtual VectorXd inFrame(const VectorXd& allCoordinates, const Shared<const Frame>& fromFrame, const Shared<const CoordinatesBroker>& aCoordinatesBroker, const Shared<const Frame>& toFrame) const override;
};

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
