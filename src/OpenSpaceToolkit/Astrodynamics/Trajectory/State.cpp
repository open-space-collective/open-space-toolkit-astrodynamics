/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

State::State(const Instant& anInstant, const Shared<const Frame>& aFrameSPtr, const VectorXd& aCoordinates)
    : instant_(anInstant),
      frameSPtr_(aFrameSPtr),
      coordinates_(aCoordinates)
{
    if (aCoordinates.size() != 6)
    {
        throw ostk::core::error::runtime::Wrong("Coordinates size");
    }
}

State::State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity)
    : instant_(anInstant)
{
    if (!aPosition.isDefined() || !aVelocity.isDefined())
    {
        throw ostk::core::error::runtime::Wrong("Argument undefined");
    }

    if (aPosition.accessFrame() != aVelocity.accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Position-Velocity Frames");
    }

    VectorXd coordinates(6);
    coordinates.segment(0, 3) = aPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    coordinates.segment(3, 3) = aVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    this->frameSPtr_ = aPosition.accessFrame();
    this->coordinates_ = coordinates;
}

bool State::operator==(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false;
    }

    return (this->instant_ == aState.instant_) && (this->frameSPtr_ == aState.frameSPtr_) &&
           (this->coordinates_ == aState.coordinates_);
}

bool State::operator!=(const State& aState) const
{
    return !((*this) == aState);
}

State State::operator+(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    return {this->instant_, this->frameSPtr_, this->coordinates_ + aState.coordinates_};
}

State State::operator-(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    return {this->instant_, this->frameSPtr_, this->coordinates_ - aState.coordinates_};
}

std::ostream& operator<<(std::ostream& anOutputStream, const State& aState)
{
    aState.print(anOutputStream);

    return anOutputStream;
}

bool State::isDefined() const
{
    return this->instant_.isDefined() && (frameSPtr_ != nullptr) && frameSPtr_->isDefined() &&
           this->coordinates_.isDefined();
}

const Instant& State::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->instant_;
}

const Shared<const Frame> State::accessFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->frameSPtr_;
}

const Position State::accessPosition() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Position(this->coordinates_.segment(0, 3), Position::Unit::Meter, this->frameSPtr_);
}

const Velocity State::accessVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Velocity(this->coordinates_.segment(3, 3), Velocity::Unit::MeterPerSecond, this->frameSPtr_);
}

const VectorXd& State::accessCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinates_;
}

Instant State::getInstant() const
{
    return this->accessInstant();
}

Shared<const Frame> State::getFrame() const
{
    return this->accessFrame();
}

Position State::getPosition() const
{
    return this->accessPosition();
}

Velocity State::getVelocity() const
{
    return this->accessVelocity();
}

VectorXd State::getCoordinates() const
{
    return this->accessCoordinates();
}

State State::inFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    const Position position = this->accessPosition().inFrame(aFrameSPtr, this->instant_);
    const Velocity velocity = this->accessVelocity().inFrame(this->accessPosition(), aFrameSPtr, this->instant_);

    return {this->instant_, position, velocity};
}

void State::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Position:" << (this->isDefined() ? this->accessPosition().toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Velocity:" << (this->isDefined() ? this->accessVelocity().toString(12) : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

State State::Undefined()
{
    return {Instant::Undefined(), Frame::Undefined(), VectorXd(6)};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
