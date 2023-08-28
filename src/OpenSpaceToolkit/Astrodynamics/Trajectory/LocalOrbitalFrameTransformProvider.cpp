/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>

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

LocalOrbitalFrameTransformProvider::LocalOrbitalFrameTransformProvider(const LocalOrbitalFrameTransformProvider::Type& aType, const LocalOrbitalFrameTransformProvider::Generator& aGenerator)
    : type_(aType),
      generator_(aGenerator)
{
}

Shared<const LocalOrbitalFrameTransformProvider> LocalOrbitalFrameTransformProvider::Construct(
    const LocalOrbitalFrameTransformProvider::Type& aType
)
{
    LocalOrbitalFrameTransformProvider::Generator generator;

    switch (aType)
    {
        case LocalOrbitalFrameTransformProvider::Type::VNC:

            generator = [](const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) -> Transform
            {
                // TBI: Check reference frame
                // TBI: Refine to use coordinates straight away
                const Vector3d transformPosition = -aPosition.getCoordinates();
                const Vector3d transformVelocity = -aVelocity.getCoordinates();
                const Vector3d xAxis = aPosition.getCoordinates().normalized();
                const Vector3d yAxis = aPosition.getCoordinates().cross(aPosition.getCoordinates()).normalized();
                const Vector3d zAxis = xAxis.cross(yAxis);
                const Quaternion transformOrientation = Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
                const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0}; // TBD

                return {
                    anInstant,
                    transformPosition,
                    transformVelocity,
                    transformOrientation,
                    transformAngularVelocity,
                    Transform::Type::Passive
                };

            };

        default:
            throw ostk::core::error::runtime::ToBeImplemented("Generate Transform");
            break;

    };

    return std::make_shared<LocalOrbitalFrameTransformProvider>(LocalOrbitalFrameTransformProvider(aType, generator));
}

LocalOrbitalFrameTransformProvider::~LocalOrbitalFrameTransformProvider() {}

LocalOrbitalFrameTransformProvider* LocalOrbitalFrameTransformProvider::clone() const
{
    return new LocalOrbitalFrameTransformProvider(*this);
}

bool LocalOrbitalFrameTransformProvider::isDefined() const
{
    return !!generator_;
}

LocalOrbitalFrameTransformProvider::Type LocalOrbitalFrameTransformProvider::getType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local orbital frame transform provider");
    }

    return type_;
}

Transform LocalOrbitalFrameTransformProvider::getTransformAt([[maybe_unused]] const Instant& anInstant) const
{
    throw ostk::core::error::runtime::Wrong("Get Transform At on Local Orbital Frame Transform Provider requires Position and Velocity input");
}

Transform LocalOrbitalFrameTransformProvider::getTransformAt(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local orbital frame transform provider");
    }

    return generator_(anInstant, aPosition, aVelocity);
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

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
