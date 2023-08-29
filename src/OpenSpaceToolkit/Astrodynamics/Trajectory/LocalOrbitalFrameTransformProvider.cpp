/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::math::geom::d3::trf::rot::RotationMatrix;
using ostk::math::geom::d3::trf::rot::Quaternion;

using ostk::physics::coord::Vector3d;
using ostk::physics::coord::frame::Transform;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

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
        case LocalOrbitalFrameTransformProvider::Type::VNC:
            const Vector3d transformPosition = -aPosition;
            const Vector3d transformVelocity = -aVelocity;
            const Vector3d xAxis = aPosition.normalized();
            const Vector3d yAxis = aPosition.cross(aPosition).normalized();
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

    throw ostk::core::error::runtime::ToBeImplemented("Generate Transform");
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
