/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{
namespace state
{
namespace coordinatessubsets
{

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

const String CartesianVelocity::DEFAULT_NAME = "CARTESIAN_VELOCITY";

CartesianVelocity::CartesianVelocity(const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName)
    : CoordinatesSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr)
{
}

CartesianVelocity::~CartesianVelocity() {}

VectorXd CartesianVelocity::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinates,
    const VectorXd& anotherFullCoordinates,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    return aCoordinatesBroker->extractCoordinates(aFullCoordinates, *this) +
           aCoordinatesBroker->extractCoordinates(anotherFullCoordinates, *this);
}

VectorXd CartesianVelocity::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinates,
    const VectorXd& anotherFullCoordinates,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    return aCoordinatesBroker->extractCoordinates(aFullCoordinates, *this) -
           aCoordinatesBroker->extractCoordinates(anotherFullCoordinates, *this);
}

VectorXd CartesianVelocity::inFrame(
    const Instant& anInstant,
    const VectorXd& allCoordinates,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    if (this->getSize() != 3)
    {
        throw ostk::core::error::runtime::ToBeImplemented("Frame Transformation");
    }

    const VectorXd positionCoordinates =
        aCoordinatesBroker->extractCoordinates(allCoordinates, this->cartesianPositionSPtr_);
    const VectorXd velocityCoordinates = aCoordinatesBroker->extractCoordinates(allCoordinates, *this);

    Vector3d toFrameCoordinates =
        Velocity::MetersPerSecond({velocityCoordinates(0), velocityCoordinates(1), velocityCoordinates(2)}, fromFrame)
            .inFrame(
                Position::Meters({positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)}, fromFrame),
                toFrame,
                anInstant
            )
            .getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
