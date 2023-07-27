/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

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

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;

const String CartesianPosition::DEFAULT_NAME = "CARTESIAN_POSITION";

CartesianPosition::CartesianPosition(const String& aName)
    : CoordinatesSubset(aName, 3)
{
}

CartesianPosition::~CartesianPosition() {}

VectorXd CartesianPosition::add(
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

VectorXd CartesianPosition::subtract(
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

VectorXd CartesianPosition::inFrame(
    const Instant& anInstant,
    const VectorXd& allCoordinates,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    VectorXd positionCoordinates = aCoordinatesBroker->extractCoordinates(allCoordinates, *this);

    Vector3d toFrameCoordinates =
        Position::Meters({positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)}, fromFrame)
            .inFrame(toFrame, anInstant)
            .getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
