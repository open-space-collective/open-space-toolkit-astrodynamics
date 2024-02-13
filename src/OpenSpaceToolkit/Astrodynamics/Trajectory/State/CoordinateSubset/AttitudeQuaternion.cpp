/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatessubset
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;

AttitudeQuaternion::AttitudeQuaternion(const String& aName)
    : CoordinateSubset(aName, 4)
{
}

AttitudeQuaternion::~AttitudeQuaternion() {}

VectorXd AttitudeQuaternion::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    const VectorXd coordinates = aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);

    const Quaternion quaternion = coordinatesToQuaternion(coordinates);

    const Transform transform = fromFrame->getTransformTo(toFrame, anInstant);

    const Quaternion quaternionInFrame = quaternion * transform.getOrientation().toConjugate();

    return quaterionToCoordinates(quaternionInFrame);
}

Quaternion AttitudeQuaternion::coordinatesToQuaternion(const VectorXd& coordinates)
{
    return {coordinates(0), coordinates(1), coordinates(2), coordinates(3), Quaternion::Format::XYZS};
}

VectorXd AttitudeQuaternion::quaterionToCoordinates(const Quaternion& quaternion)
{
    VectorXd coordinates(4);
    coordinates << quaternion.x(), quaternion.y(), quaternion.z(), quaternion.s();

    return coordinates;
}

Shared<const AttitudeQuaternion> AttitudeQuaternion::Default()
{
    static const Shared<const AttitudeQuaternion> attitudeQuaternion =
        std::make_shared<AttitudeQuaternion>("ATTITUDE_QUATERNION");
    return attitudeQuaternion;
}

}  // namespace coordinatessubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
