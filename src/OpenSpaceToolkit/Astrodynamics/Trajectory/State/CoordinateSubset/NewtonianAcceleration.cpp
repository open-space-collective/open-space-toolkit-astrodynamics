/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/NewtonianAcceleration.hpp>

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

NewtonianAcceleration::NewtonianAcceleration(
    const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName
)
    : CoordinateSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr)
{
}

NewtonianAcceleration::~NewtonianAcceleration() {}

VectorXd NewtonianAcceleration::add(
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

VectorXd NewtonianAcceleration::subtract(
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

VectorXd NewtonianAcceleration::inFrame(
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

Shared<const NewtonianAcceleration> NewtonianAcceleration::Default()
{
    static const Shared<const NewtonianAcceleration> cartesianAcceleration =
        std::make_shared<NewtonianAcceleration>(CartesianPosition::Default(), "CARTESIAN_ACCELERATION");

    return cartesianAcceleration;
}

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
