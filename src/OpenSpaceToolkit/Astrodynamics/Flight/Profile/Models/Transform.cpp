/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Transform.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{
namespace models
{

Transform::Transform(const DynamicProvider& aDynamicTransformProvider, const Shared<const Frame>& aFrameSPtr)
    : transformProvider_(aDynamicTransformProvider),
      frameSPtr_(aFrameSPtr)
{
}

Transform* Transform::clone() const
{
    return new Transform(*this);
}

bool Transform::isDefined() const
{
    return this->transformProvider_.isDefined() && (this->frameSPtr_ != nullptr) && this->frameSPtr_->isDefined();
}

State Transform::calculateStateAt(const Instant& anInstant) const
{
    using ostk::physics::coordinate::Position;
    using ostk::physics::coordinate::Transform;
    using ostk::physics::coordinate::Velocity;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Transform");
    }

    const Transform T_REF_B = this->transformProvider_.getTransformAt(anInstant);

    const Quaternion q_REF_B = T_REF_B.getOrientation();
    const Vector3d w_REF_B_in_REF = T_REF_B.getAngularVelocity();

    const Vector3d x_REF = T_REF_B.applyToPosition({0.0, 0.0, 0.0});
    const Vector3d v_REF_in_REF = T_REF_B.applyToVelocity({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
    const Quaternion q_B_REF = q_REF_B.toConjugate();
    const Vector3d w_B_REF_in_B = q_B_REF * (w_REF_B_in_REF * -1.0);

    return {
        anInstant,
        Position::Meters(x_REF, this->frameSPtr_),
        Velocity::MetersPerSecond(v_REF_in_REF, this->frameSPtr_),
        q_B_REF,
        w_B_REF_in_B,
        this->frameSPtr_
    };
}

Axes Transform::getAxesAt(const Instant& anInstant) const
{
    using ostk::physics::coordinate::Transform;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Transform");
    }

    const Transform transform = this->transformProvider_.getTransformAt(anInstant);

    const Quaternion q_GCRF_BODY = transform.getOrientation();

    const Vector3d xAxis = q_GCRF_BODY * Vector3d::X();
    const Vector3d yAxis = q_GCRF_BODY * Vector3d::Y();
    const Vector3d zAxis = q_GCRF_BODY * Vector3d::Z();

    return {xAxis, yAxis, zAxis, this->frameSPtr_};
}

Shared<const Frame> Transform::getBodyFrame(const String& aFrameName) const
{
    using ostk::physics::coordinate::Transform;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Transform");
    }

    const DynamicProvider dynamicTransformProvider = {
        [this](const Instant& anInstant) -> Transform
        {
            return this->transformProvider_.getTransformAt(anInstant).getInverse();
        }
    };

    return Frame::Construct(
        aFrameName, false, this->frameSPtr_, std::make_shared<const DynamicProvider>(dynamicTransformProvider)
    );
}

void Transform::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Transform") : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Transform Transform::Undefined()
{
    return {DynamicProvider::Undefined(), Frame::Undefined()};
}

Transform Transform::InertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion)
{
    using ostk::physics::coordinate::Transform;

    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    if (!aQuaternion.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Quaternion");
    }

    const DynamicProvider dynamicTransformProvider = {
        [aTrajectory, aQuaternion](const Instant& anInstant) -> Transform
        {
            static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

            const trajectory::State trajectoryState = aTrajectory.getStateAt(anInstant).inFrame(gcrfSPtr);

            const Vector3d x_GCRF = trajectoryState.getPosition().getCoordinates();
            const Vector3d v_GCRF = trajectoryState.getVelocity().getCoordinates();

            return Transform::Passive(anInstant, x_GCRF, v_GCRF, aQuaternion, {0.0, 0.0, 0.0});
        }
    };

    return {dynamicTransformProvider, Frame::GCRF()};
}

Transform Transform::NadirPointing(
    const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
)
{
    using ostk::physics::coordinate::Transform;

    if (!anOrbit.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    if (anOrbitalFrameType == trajectory::Orbit::FrameType::Undefined)
    {
        throw ostk::core::error::runtime::Undefined("Orbital frame type");
    }

    const Shared<const Frame> orbitalFrameSPtr = anOrbit.getOrbitalFrame(anOrbitalFrameType);

    const DynamicProvider dynamicTransformProvider = {
        [anOrbit, orbitalFrameSPtr](const Instant& anInstant) -> Transform
        {
            static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

            return orbitalFrameSPtr->getTransformTo(gcrfSPtr, anInstant);
        }
    };

    return {dynamicTransformProvider, Frame::GCRF()};
}

bool Transform::operator==(const Model& aModel) const
{
    if ((!this->isDefined()) || (!aModel.isDefined()))
    {
        return false;
    }

    const Transform* transformModelPtr = dynamic_cast<const Transform*>(&aModel);

    throw ostk::core::error::runtime::ToBeImplemented("Transform::operator ==");

    return (this->frameSPtr_ == transformModelPtr->frameSPtr_);
}

}  // namespace models
}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk
