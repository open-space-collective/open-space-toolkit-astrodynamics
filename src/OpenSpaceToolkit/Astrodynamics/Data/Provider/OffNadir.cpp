/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Data/Provider/OffNadir.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace data
{
namespace provider
{

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;

static const Shared<const LocalOrbitalFrameFactory> VVLHFrameFactory = LocalOrbitalFrameFactory::VVLH(Frame::GCRF());

Tuple<Angle, Angle, Angle> ComputeOffNadirAngles(const State& aState, const Position& aTargetPosition)
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
    const Shared<const Frame> parentFrameSPtr = VVLHFrameFactory->accessParentFrame();
    const State stateInFrame = aState.inFrame(parentFrameSPtr);
    
    const Shared<const Frame> localOrbitalFrameSPtr = VVLHFrameFactory->generateFrame(
        instant, stateInFrame.getPosition().getCoordinates(), stateInFrame.getVelocity().getCoordinates()
    );

    // Calculate satellite to target direction vector in parent frame
    const Vector3d satelliteToTargetDirection =
        (aTargetPosition.inFrame(parentFrameSPtr, instant).inMeters().getCoordinates() -
         aState.inFrame(parentFrameSPtr).getPosition().inMeters().getCoordinates())
            .normalized();

    // Transform direction vector to local orbital frame
    const Vector3d localDirection =
        parentFrameSPtr->getTransformTo(localOrbitalFrameSPtr, instant).applyToVector(satelliteToTargetDirection);

    const double x = localDirection.x();
    const double y = localDirection.y();
    const double z = localDirection.z();

    return {
        Angle::Radians(std::atan2(x, z)),  // Along-track angle
        Angle::Radians(std::atan2(y, z)),  // Cross-track angle
        Angle::Radians(std::acos(z))       // Total off-nadir angle
    };
}

}  // namespace provider
}  // namespace data
}  // namespace astrodynamics
}  // namespace ostk
