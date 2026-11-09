/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

ConstantThrust::ConstantThrust(const LocalOrbitalFrameDirection& aThrustDirection)
    : GuidanceLaw("Constant Thrust"),
      localOrbitalFrameDirection_(aThrustDirection)
{
}

ConstantThrust::~ConstantThrust() {}

std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrust& aConstantThrust)
{
    aConstantThrust.print(anOutputStream);

    return anOutputStream;
}

const LocalOrbitalFrameDirection& ConstantThrust::getLocalThrustDirection() const
{
    return localOrbitalFrameDirection_;
}

Vector3d ConstantThrust::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    double aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const Shared<const Frame> parentFrameSPtr =
        this->localOrbitalFrameDirection_.getLocalOrbitalFrameFactory().getParentFrame();

    const State state = {
        anInstant,
        Position::Meters(aPositionCoordinates, parentFrameSPtr),
        Velocity::MetersPerSecond(aVelocityCoordinates, parentFrameSPtr),
    };

    const Shared<const Frame> localOrbitalFrameSPtr =
        this->localOrbitalFrameDirection_.getLocalOrbitalFrameFactory().generateFrame(state);

    const Quaternion q_requestedFrame_LOF =
        localOrbitalFrameSPtr->getTransformTo(outputFrameSPtr, anInstant).getOrientation().toNormalized();

    const Vector3d acceleration_LOF = aThrustAcceleration * this->localOrbitalFrameDirection_.getValue();

    return q_requestedFrame_LOF.rotateVector(acceleration_LOF);
}

void ConstantThrust::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Constant Thrust Guidance Law") : void();

    GuidanceLaw::print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Thrust Direction");
    localOrbitalFrameDirection_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

ConstantThrust ConstantThrust::Intrack(const bool& velocityDirection)
{
    const Vector3d direction = (velocityDirection ? Vector3d {1.0, 0.0, 0.0} : Vector3d {-1.0, 0.0, 0.0});
    const LocalOrbitalFrameDirection localOrbitalFrameDirection =
        LocalOrbitalFrameDirection(direction, LocalOrbitalFrameFactory::VNC(Frame::GCRF()));

    return {localOrbitalFrameDirection};
}

ConstantThrust ConstantThrust::FromManeuver(
    const Maneuver& aManeuver,
    const LocalOrbitalFrameFactory& aLocalOrbitalFrameFactory,
    const std::optional<Angle>& aMaximumAllowedAngularOffset
)
{
    const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
        aManeuver.calculateMeanThrustDirectionAndMaximumAngularOffset(aLocalOrbitalFrameFactory);

    if (aMaximumAllowedAngularOffset.has_value())
    {
        const double maximumAllowedAngularOffsetDegrees = aMaximumAllowedAngularOffset->inDegrees(0.0, 360.0);
        const double maximumAngularOffsetDegrees = meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0);
        if (maximumAngularOffsetDegrees > maximumAllowedAngularOffsetDegrees)
        {
            throw ostk::core::error::RuntimeError(String::Format(
                "Maximum angular offset ({:.6f} deg) is greater than the maximum allowed ({:.6f} deg).",
                maximumAngularOffsetDegrees,
                maximumAllowedAngularOffsetDegrees
            ));
        }
    }

    return {meanDirectionAndMaximumAngularOffset.first};
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
