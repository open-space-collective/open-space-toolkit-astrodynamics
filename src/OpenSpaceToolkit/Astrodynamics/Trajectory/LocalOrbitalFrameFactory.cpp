/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

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
) const
{
    const String name = this->generateFrameName(anInstant, aPosition, aVelocity);

    const Shared<const LocalOrbitalFrameTransformProvider> providerSPtr_ =
        LocalOrbitalFrameTransformProvider::Construct(this->type_, anInstant, aPosition, aVelocity);

    if (const auto frameSPtr = FrameManager::Get().accessFrameWithName(name))
    {
        return frameSPtr;
    }

    const Shared<const Frame> frameSPtr =
        std::make_shared<const SharedFrameEnabler>(name, false, parentFrameSPtr_, providerSPtr_);

    FrameManager::Get().addFrame(frameSPtr);

    return frameSPtr;
}

bool LocalOrbitalFrameFactory::isDefined() const
{
    return (parentFrameSPtr_ != nullptr) && parentFrameSPtr_->isDefined() &&
           (type_ != LocalOrbitalFrameTransformProvider::Type::Undefined);
}

LocalOrbitalFrameTransformProvider::Type LocalOrbitalFrameFactory::getProviderType() const
{
    return type_;
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::NED(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::NED, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::LVLH(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::LVLH, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::VVLH(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::VVLH, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::QSW(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::QSW, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::TNW(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::TNW, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::VNC(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::VNC, aParentFrame);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::Undefined()
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::Undefined, nullptr);
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameFactory::Construct(
    const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
)
{
    return std::make_shared<LocalOrbitalFrameFactory>(LocalOrbitalFrameFactory(aType, aParentFrame));
}

LocalOrbitalFrameFactory::LocalOrbitalFrameFactory(
    const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
)
{
    this->type_ = aType;
    this->parentFrameSPtr_ = aParentFrame;
}

String LocalOrbitalFrameFactory::generateFrameName(
    const Instant& anInstant, const Vector3d& aPosition, const Vector3d& aVelocity
) const
{
    return LocalOrbitalFrameTransformProvider::StringFromType(type_) + "@" + anInstant.toString() +
           aPosition.toString() + aVelocity.toString();
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
