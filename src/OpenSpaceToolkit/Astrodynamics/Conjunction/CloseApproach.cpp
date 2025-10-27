/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility/Print.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::container::Array;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

CloseApproach::CloseApproach(const State& anObject1State, const State& anObject2State)
    : object1State_(anObject1State),
      object2State_(anObject2State),
      stateBuilder_(StateBuilder::Undefined())
{
    if (anObject1State.isDefined() && anObject2State.isDefined())
    {
        if (anObject1State.getInstant() != anObject2State.getInstant())
        {
            throw ostk::core::error::RuntimeError("Inconsistent state instants.");
        }

        // Build state builder with the frame of Object 1 and position/velocity coordinate subsets
        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };

        stateBuilder_ = StateBuilder(anObject1State.accessFrame(), coordinateSubsets);
    }
}

bool CloseApproach::operator==(const CloseApproach& aCloseApproach) const
{
    if ((!this->isDefined()) || (!aCloseApproach.isDefined()))
    {
        return false;
    }

    return (object1State_ == aCloseApproach.object1State_) && (object2State_ == aCloseApproach.object2State_);
}

bool CloseApproach::operator!=(const CloseApproach& aCloseApproach) const
{
    return !((*this) == aCloseApproach);
}

std::ostream& operator<<(std::ostream& anOutputStream, const CloseApproach& aCloseApproach)
{
    aCloseApproach.print(anOutputStream);

    return anOutputStream;
}

bool CloseApproach::isDefined() const
{
    return object1State_.isDefined() && object2State_.isDefined() && stateBuilder_.isDefined();
}

State CloseApproach::getObject1State() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object1State_;
}

State CloseApproach::getObject2State() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object2State_;
}

Instant CloseApproach::getInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object1State_.getInstant();
}

Length CloseApproach::getMissDistance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const State relativeState = this->getRelativeState();
    const Position relativePosition = relativeState.getPosition();
    const Vector3d relativePositionCoordinates = relativePosition.inMeters().getCoordinates();

    return Length::Meters(relativePositionCoordinates.norm());
}

State CloseApproach::getRelativeState() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const State reducedObject1State = stateBuilder_.reduce(object1State_);
    const State reducedObject2State = stateBuilder_.reduce(object2State_.inFrame(object1State_.accessFrame()));

    return reducedObject2State - reducedObject1State;
}

Tuple<Length, Length, Length> CloseApproach::computeMissDistanceComponentsInFrame(const Shared<const Frame>& aFrame
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    if ((aFrame == nullptr) || (!aFrame->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    const State relativeStateInFrame = this->getRelativeState().inFrame(aFrame);
    const Position relativePosition = relativeStateInFrame.getPosition();
    const Vector3d coordinates = relativePosition.inMeters().getCoordinates();

    return {Length::Meters(coordinates[0]), Length::Meters(coordinates[1]), Length::Meters(coordinates[2])};
}

Tuple<Length, Length, Length> CloseApproach::computeMissDistanceComponentsInLocalOrbitalFrame(
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    if ((aLocalOrbitalFrameFactorySPtr == nullptr) || (!aLocalOrbitalFrameFactorySPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("LocalOrbitalFrameFactory");
    }

    const Shared<const Frame> localOrbitalFrame = aLocalOrbitalFrameFactorySPtr->generateFrame(object1State_);
    const State relativeStateInFrame = object2State_.inFrame(localOrbitalFrame);
    const Position relativePosition = relativeStateInFrame.getPosition();
    const Vector3d coordinates = relativePosition.inMeters().getCoordinates();

    return {Length::Meters(coordinates[0]), Length::Meters(coordinates[1]), Length::Meters(coordinates[2])};
}

void CloseApproach::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Close Approach") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->isDefined() ? this->getInstant().toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Miss Distance:" << (this->isDefined() ? this->getMissDistance().toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Object 1 State:" << (object1State_.isDefined() ? "Defined" : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Object 2 State:" << (object2State_.isDefined() ? "Defined" : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

CloseApproach CloseApproach::Undefined()
{
    return CloseApproach(State::Undefined(), State::Undefined());
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
