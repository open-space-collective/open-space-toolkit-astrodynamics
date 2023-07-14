/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

CartesianPosition::CartesianPosition() : CoordinatesSubset("CARTESIAN_POSITION", 3)
{
}

VectorXd CartesianPosition::inFrame(const VectorXd& allCoordinates, const Shared<const Frame>& fromFrame, const Shared<const CoordinatesBroker>& aCoordinatesBroker, const Shared<const Frame>& toFrame) const {
    return VectorXd();
}

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
