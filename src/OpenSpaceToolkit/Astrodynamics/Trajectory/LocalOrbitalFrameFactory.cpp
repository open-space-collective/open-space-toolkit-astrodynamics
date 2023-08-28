/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utilities.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::math::geom::d3::trf::rot::Quaternion;
using ostk::math::geom::d3::trf::rot::RotationMatrix;
using ostk::math::geom::d3::trf::rot::RotationVector;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using FrameManager = ostk::physics::coord::frame::Manager;
using ostk::physics::coord::Transform;
using ostk::physics::coord::frame::Provider;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;

struct SharedFrameEnabler : public Frame
{
    SharedFrameEnabler(
        const String& aName,
        bool isQuasiInertial,
        const Shared<const Frame>& aParentFrame,
        const Shared<const Provider>& aProvider
    )
        : Frame(aName, isQuasiInertial, aParentFrame, aProvider)
    {
    }
};

Shared<const Frame> LocalOrbitalFrameFactory::generateFrame(
    const Instant& anInstant, const Vector3d& aPosition, const Vector3d& aVelocity
)
{
    // Uniqueness clashing risk here (see note in function 'generateFrameName')
    const String name = this->generateFrameName(anInstant, aPosition, aVelocity);

    if (const auto frameSPtr = FrameManager::Get().accessFrameWithName(name))
    {
        return frameSPtr;
    }

    const Shared<const Frame> frameSPtr = std::make_shared<const SharedFrameEnabler>(
        name, false, parentFrameSPtr_, std::dynamic_pointer_cast<const Provider>(providerSPtr_)
    );

    FrameManager::Get().addFrame(frameSPtr);

    return frameSPtr;
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::Construct(
    const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
)
{
    return std::make_shared<LocalOrbitalFrameFactory>(LocalOrbitalFrameFactory(aType, aParentFrame));
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::VNC(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::VNC, aParentFrame);
}

LocalOrbitalFrameFactory::LocalOrbitalFrameFactory(
    const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
)
{
    this->providerSPtr_ = LocalOrbitalFrameTransformProvider::Construct(aType);
    this->parentFrameSPtr_ = aParentFrame;
}

String LocalOrbitalFrameFactory::generateFrameName(
    const Instant& anInstant, [[maybe_unused]] const Vector3d& aPosition, [[maybe_unused]] const Vector3d& aVelocity
)
{
    // !! Since frames are being emplaced using only the instant and type is not 100% robust
    // We should use parent frame, type, isntant, pos, vel to ensure uniqueness of the frame name
    // Otherwise, 2 satellites with different parent frame, type, pos, vel will generate a clash in frames
    // just because they are at the same instant
    return LocalOrbitalFrameTransformProvider::StringFromType(this->providerSPtr_->getType()) + "@" +
           anInstant.toString();
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
