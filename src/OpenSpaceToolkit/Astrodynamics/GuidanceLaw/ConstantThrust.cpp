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

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

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

LocalOrbitalFrameDirection ConstantThrust::getLocalThrustDirection() const
{
    if (!localOrbitalFrameDirection_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local Thrust Direction");
    }

    return localOrbitalFrameDirection_;
}

Vector3d ConstantThrust::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const Shared<const Frame> localOrbitalFrameSPtr =
        this->localOrbitalFrameDirection_.accessLocalOrbitalFrameFactory()->generateFrame(
            anInstant, aPositionCoordinates, aVelocityCoordinates
        );

    const Quaternion q_requestedFrame_LOF =
        localOrbitalFrameSPtr->getTransformTo(outputFrameSPtr, anInstant).getOrientation().normalize();

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

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
