/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utility.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using FrameManager = ostk::physics::coordinate::frame::Manager;
using ostk::physics::coordinate::frame::Provider;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

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

bool LocalOrbitalFrameFactory::operator==(const LocalOrbitalFrameFactory& aLocalOrbitalFrameFactory) const
{
    if ((!this->isDefined()) || (!aLocalOrbitalFrameFactory.isDefined()))
    {
        return false;
    }

    if (type_ == LocalOrbitalFrameTransformProvider::Type::Custom)
    {
        return this == &aLocalOrbitalFrameFactory;
    }

    return (type_ == aLocalOrbitalFrameFactory.type_) &&
           (parentFrameSPtr_ == aLocalOrbitalFrameFactory.parentFrameSPtr_);
}

bool LocalOrbitalFrameFactory::operator!=(const LocalOrbitalFrameFactory& aLocalOrbitalFrameFactory) const
{
    return !((*this) == aLocalOrbitalFrameFactory);
}

Shared<const Frame> LocalOrbitalFrameFactory::generateFrame(const State& aState) const
{
    const StateBuilder positionVelocityStateBuilder =
        StateBuilder(aState.getFrame(), {CartesianPosition::Default(), CartesianVelocity::Default()});

    const State positionVelocityStateInParentFrame =
        positionVelocityStateBuilder.reduce(aState).inFrame(parentFrameSPtr_);

    const String name = this->generateFrameName(positionVelocityStateInParentFrame);

    if (const auto frameSPtr = FrameManager::Get().accessFrameWithName(name))
    {
        return frameSPtr;
    }

    const Transform transform = transformGenerator_(positionVelocityStateInParentFrame);

    const Shared<const LocalOrbitalFrameTransformProvider> providerSPtr =
        std::make_shared<const LocalOrbitalFrameTransformProvider>(transform);

    const Shared<const Frame> frameSPtr =
        std::make_shared<const SharedFrameEnabler>(name, false, parentFrameSPtr_, providerSPtr);

    FrameManager::Get().addFrame(frameSPtr);

    return frameSPtr;
}

bool LocalOrbitalFrameFactory::isDefined() const
{
    return (parentFrameSPtr_ != nullptr) && parentFrameSPtr_->isDefined() &&
           (type_ != LocalOrbitalFrameTransformProvider::Type::Undefined);
}

const Shared<const Frame>& LocalOrbitalFrameFactory::accessParentFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local Orbital Frame Factory");
    }

    return this->parentFrameSPtr_;
}

LocalOrbitalFrameTransformProvider::Type LocalOrbitalFrameFactory::getProviderType() const
{
    return type_;
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::NED(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::NED, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::LVLH(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::LVLH, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::VVLH(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::VVLH, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::QSW(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::QSW, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::TNW(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::TNW, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::VNC(const Shared<const Frame>& aParentFrame)
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::VNC, aParentFrame);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::Undefined()
{
    return Construct(LocalOrbitalFrameTransformProvider::Type::Undefined, nullptr);
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::Construct(
    const LocalOrbitalFrameTransformProvider::Type& aType, const Shared<const Frame>& aParentFrame
)
{
    if (aType == LocalOrbitalFrameTransformProvider::Type::Custom)
    {
        throw ostk::core::error::RuntimeError("Must provide a transform generator for custom frame type.");
    }

    return LocalOrbitalFrameFactory(
        aType, aParentFrame, LocalOrbitalFrameTransformProvider::GetTransformGenerator(aType)
    );
}

LocalOrbitalFrameFactory LocalOrbitalFrameFactory::Construct(
    const TransformGenerator& aTransformGenerator, const Shared<const Frame>& aParentFrame
)
{
    return LocalOrbitalFrameFactory(
        LocalOrbitalFrameTransformProvider::Type::Custom, aParentFrame, aTransformGenerator
    );
}

LocalOrbitalFrameFactory::LocalOrbitalFrameFactory(
    const LocalOrbitalFrameTransformProvider::Type& aType,
    const Shared<const Frame>& aParentFrame,
    const TransformGenerator& aTransformGenerator
)
    : type_(aType),
      parentFrameSPtr_(aParentFrame),
      transformGenerator_(aTransformGenerator)
{
}

String LocalOrbitalFrameFactory::generateFrameName(const State& aState) const
{
    return LocalOrbitalFrameTransformProvider::StringFromType(type_) + "@" + aState.accessInstant().toString() +
           aState.getPosition().getCoordinates().toString() + aState.getVelocity().getCoordinates().toString();
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
