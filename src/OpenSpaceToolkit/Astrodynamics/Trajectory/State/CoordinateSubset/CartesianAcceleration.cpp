/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>

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
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;

CartesianAcceleration::CartesianAcceleration(
    const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName
)
    : CoordinateSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr)
{
}

CartesianAcceleration::~CartesianAcceleration() {}

VectorXd CartesianAcceleration::add(
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

VectorXd CartesianAcceleration::subtract(
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

VectorXd CartesianAcceleration::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    const Vector3d accelerationCoordinates =
        Vector3d::Map(aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this).data());

    // As per
    // https://space.stackexchange.com/questions/58123/conversion-of-acceleration-in-inertial-frame-to-non-inertial-rotating-frame
    // acceleration due to gravity is the same in all Newtonian frames
    const Transform transform = fromFrame->getTransformTo(toFrame, anInstant);

    return VectorXd::Map(transform.applyToVector(accelerationCoordinates).data(), Eigen::Index(3));
}

Shared<const CartesianAcceleration> CartesianAcceleration::Default()
{
    static const Shared<const CartesianAcceleration> cartesianAcceleration =
        std::make_shared<CartesianAcceleration>(CartesianPosition::Default(), "CARTESIAN_ACCELERATION");

    return cartesianAcceleration;
}

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
