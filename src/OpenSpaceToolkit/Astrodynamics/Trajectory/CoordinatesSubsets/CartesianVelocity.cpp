/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

const String CartesianVelocity::DEFAULT_NAME = "CARTESIAN_VELOCITY";

CartesianVelocity::CartesianVelocity(const String& aName, const CartesianPosition& aCartesianPosition)
    : CoordinatesSubset(aName, aCartesianPosition.getSize()),
    cartesianPosition_(aCartesianPosition)
{
}

VectorXd CartesianVelocity::inFrame(const Instant& anInstant, const VectorXd& allCoordinates, const Shared<const Frame>& fromFrame, const Shared<const CoordinatesBroker>& aCoordinatesBroker, const Shared<const Frame>& toFrame) const
{
    if (this->getSize() != 3) {
        throw ostk::core::error::runtime::ToBeImplemented("Frame Transformation");
    }

    const VectorXd positionCoordinates = aCoordinatesBroker->extract(allCoordinates, this->cartesianPosition_);
    const VectorXd velocityCoordinates = aCoordinatesBroker->extract(allCoordinates, *this);

    Vector3d toFrameCoordinates = Velocity::MetersPerSecond(
        {velocityCoordinates(0), velocityCoordinates(1), velocityCoordinates(2)},
        fromFrame
    ).inFrame(
        Position::Meters(
            {positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)},
            fromFrame),
        toFrame,
        anInstant
    ).getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

CartesianVelocity CartesianVelocity::FromPosition(const CartesianPosition& aCartesianPosition)
{
    return {CartesianVelocity::DEFAULT_NAME, aCartesianPosition};
}

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
