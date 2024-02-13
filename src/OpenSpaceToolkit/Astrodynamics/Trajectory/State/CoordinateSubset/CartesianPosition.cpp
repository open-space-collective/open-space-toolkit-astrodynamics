/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatesubset
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;

CartesianPosition::CartesianPosition(const String& aName)
    : CoordinateSubset(aName, 3)
{
}

CartesianPosition::~CartesianPosition() {}

VectorXd CartesianPosition::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    return aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this) +
           aCoordinateBrokerSPtr->extractCoordinate(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianPosition::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    return aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this) -
           aCoordinateBrokerSPtr->extractCoordinate(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianPosition::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    VectorXd positionCoordinates = aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);

    Vector3d toFrameCoordinates =
        Position::Meters({positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)}, fromFrame)
            .inFrame(toFrame, anInstant)
            .getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

Shared<const CartesianPosition> CartesianPosition::Default()
{
    static const Shared<const CartesianPosition> cartesianPosition =
        std::make_shared<CartesianPosition>("CARTESIAN_POSITION");
    return cartesianPosition;
}

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
