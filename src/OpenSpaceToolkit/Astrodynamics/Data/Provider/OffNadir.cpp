/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Data/Provider/OffNadir.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;

static const LocalOrbitalFrameFactory VVLH_FRAME_FACTORY = LocalOrbitalFrameFactory::VVLH(Frame::GCRF());

Triple<Angle, Angle, Angle> ComputeOffNadirAngles(const State& aState, const Position& aTargetPosition)
{
    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (!aTargetPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Target position");
    }

    const Instant instant = aState.getInstant();

    const Shared<const Frame> localOrbitalFrameSPtr = VVLH_FRAME_FACTORY.generateFrame(
        instant, aState.getPosition().getCoordinates(), aState.getVelocity().getCoordinates()
    );

    // Calculate satellite to target direction vector in GCRF frame
    const Vector3d satelliteToTargetDirection =
        (aTargetPosition.inFrame(Frame::GCRF(), instant).inMeters().getCoordinates() -
         aState.inFrame(Frame::GCRF()).getPosition().inMeters().getCoordinates())
            .normalized();

    // Transform direction vector to local orbital frame
    const Vector3d localDirection =
        Frame::GCRF()->getTransformTo(localOrbitalFrameSPtr, instant).applyToVector(satelliteToTargetDirection);

    const double x = localDirection.x();
    const double y = localDirection.y();
    const double z = localDirection.z();

    return std::make_tuple(
        Angle::Radians(std::atan2(x, z)),  // Along-track angle
        Angle::Radians(std::atan2(y, z)),  // Cross-track angle
        Angle::Radians(std::acos(z))       // Total off-nadir angle
    );
}
