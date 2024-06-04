/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utility.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;

using ostk::physics::coordinate::frame::Transform;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Vector3d;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::gravitational::Earth;

Shared<const LocalOrbitalFrameTransformProvider> LocalOrbitalFrameTransformProvider::Construct(
    const LocalOrbitalFrameTransformProvider::Type& aType,
    const Instant& anInstant,
    const Vector3d& aPosition,
    const Vector3d& aVelocity
)
{
    const Transform transform =
        LocalOrbitalFrameTransformProvider::generateTransform(aType, anInstant, aPosition, aVelocity);

    return std::make_shared<LocalOrbitalFrameTransformProvider>(LocalOrbitalFrameTransformProvider(transform));
}

LocalOrbitalFrameTransformProvider::~LocalOrbitalFrameTransformProvider() {}

LocalOrbitalFrameTransformProvider* LocalOrbitalFrameTransformProvider::clone() const
{
    return new LocalOrbitalFrameTransformProvider(*this);
}

bool LocalOrbitalFrameTransformProvider::isDefined() const
{
    return transform_.isDefined();
}

Transform LocalOrbitalFrameTransformProvider::getTransformAt(const Instant& anInstant) const
{
    // This is only optional, we can allow it if we want to support "frozen lof concept".
    if (anInstant != transform_.getInstant())
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    return transform_;
}

String LocalOrbitalFrameTransformProvider::StringFromType(const LocalOrbitalFrameTransformProvider::Type& aType)
{
    switch (aType)
    {
        case LocalOrbitalFrameTransformProvider::Type::Undefined:
            return "Undefined";

        case LocalOrbitalFrameTransformProvider::Type::NED:
            return "NED";

        case LocalOrbitalFrameTransformProvider::Type::LVLH:
            return "LVLH";

        case LocalOrbitalFrameTransformProvider::Type::VVLH:
            return "VVLH";

        case LocalOrbitalFrameTransformProvider::Type::LVLHGD:
            return "LVLHGD";

        case LocalOrbitalFrameTransformProvider::Type::QSW:
            return "QSW";

        case LocalOrbitalFrameTransformProvider::Type::TNW:
            return "TNW";

        case LocalOrbitalFrameTransformProvider::Type::VNC:
            return "VNC";

        default:
            throw ostk::core::error::runtime::Wrong("Frame type");
            break;
    }

    return String::Empty();
}

LocalOrbitalFrameTransformProvider::LocalOrbitalFrameTransformProvider(const Transform& aTransform)
    : transform_(aTransform)
{
}

Transform LocalOrbitalFrameTransformProvider::generateTransform(
    const LocalOrbitalFrameTransformProvider::Type& aType,
    const Instant& anInstant,
    const Vector3d& aPosition,
    const Vector3d& aVelocity
)
{
    switch (aType)
    {
        case LocalOrbitalFrameTransformProvider::Type::NED:
        {
            const LLA lla = LLA::Cartesian(aPosition, Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_);

            // Compute the NED frame to central body centered, central body fixed frame transform at position

            const Transform transform = ostk::physics::coordinate::frame::utilities::NorthEastDownTransformAt(
                lla, Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_
            );

            return transform;
        }

        case LocalOrbitalFrameTransformProvider::Type::LVLH:
        {
            // X axis along position vector
            // Z axis along orbital momentum
            // Y axis toward velocity vector
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d xAxis = aPosition.normalized();
            const Vector3d zAxis = aPosition.cross(aVelocity).normalized();
            const Vector3d yAxis = zAxis.cross(xAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};  // TBD

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        }

        case LocalOrbitalFrameTransformProvider::Type::VVLH:
        {
            // Z axis along negative position vector
            // Y axis along negative orbital momentum
            // X axis toward velocity vector
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d zAxis = -aPosition.normalized();
            const Vector3d yAxis = -aPosition.cross(aVelocity).normalized();
            const Vector3d xAxis = yAxis.cross(zAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};  // TBD

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        }

        case LocalOrbitalFrameTransformProvider::Type::QSW:
        {
            // X axis along position vector
            // Z axis along orbital momentum
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d xAxis = aPosition.normalized();
            const Vector3d zAxis = aPosition.cross(aVelocity).normalized();
            const Vector3d yAxis = zAxis.cross(xAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};  // TBD

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        }

        case LocalOrbitalFrameTransformProvider::Type::TNW:
        {
            // X axis along velocity vector
            // Z axis along orbital momentum
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d xAxis = aVelocity.normalized();
            const Vector3d zAxis = aPosition.cross(aVelocity).normalized();
            const Vector3d yAxis = zAxis.cross(xAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};  // TBD

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        }

        case LocalOrbitalFrameTransformProvider::Type::VNC:
        {
            // X axis along velocity vector
            // Y axis along orbital momentum
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d xAxis = aVelocity.normalized();
            const Vector3d yAxis = aPosition.cross(aVelocity).normalized();
            const Vector3d zAxis = xAxis.cross(yAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};  // TBD

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        }

        case LocalOrbitalFrameTransformProvider::Type::LVLHGD:
        {
            throw ostk::core::error::runtime::ToBeImplemented("Generate transform LVLHGD");
            break;
        }

        default:
        {
            throw ostk::core::error::runtime::Wrong("Local Orbital Frame type");
            break;
        }
    }
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
