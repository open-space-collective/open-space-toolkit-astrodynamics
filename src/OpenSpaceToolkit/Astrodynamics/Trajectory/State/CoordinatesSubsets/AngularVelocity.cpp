/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AngularVelocity.hpp>

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

using ostk::math::object::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

AngularVelocity::AngularVelocity(const Shared<const AttitudeQuaternion>& aAttitudeQuaternionSPtr, const String& aName)
    : CoordinatesSubset(aName, 3),
      attitudeQuaternionSPtr_(aAttitudeQuaternionSPtr)
{
}

AngularVelocity::~AngularVelocity() {}

VectorXd AngularVelocity::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    const VectorXd coordinates = aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);

    const VectorXd attitudeCoordinatesInFrame = this->attitudeQuaternionSPtr_->inFrame(
        anInstant, aFullCoordinatesVector, fromFrame, toFrame, aCoordinatesBrokerSPtr
    );
    const Quaternion quaternionInFrame =
        AttitudeQuaternion::coordinatesToQuaternion(attitudeCoordinatesInFrame).toNormalized();

    const Transform transform = fromFrame->getTransformTo(toFrame, anInstant);

    const Vector3d coordinatesInFrame = coordinates - quaternionInFrame * transform.getAngularVelocity();
    return VectorXd::Map(coordinatesInFrame.data(), static_cast<Eigen::Index>(3));
}

Shared<const AngularVelocity> AngularVelocity::Default()
{
    static const Shared<const AngularVelocity> angularVelocity =
        std::make_shared<AngularVelocity>(AttitudeQuaternion::Default(), "ANGULAR_VELOCITY");
    return angularVelocity;
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
