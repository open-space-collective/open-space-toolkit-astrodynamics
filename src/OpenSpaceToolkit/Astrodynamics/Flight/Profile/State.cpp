/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{

State::State(
    const Instant& anInstant,
    const Position& aPosition,
    const Velocity& aVelocity,
    const Quaternion& anAttitude,
    const Vector3d& anAngularVelocity,
    const Shared<const Frame>& aReferenceFrame
)
    : instant_(anInstant),
      position_(aPosition),
      velocity_(aVelocity),
      attitude_(anAttitude),
      angularVelocity_(anAngularVelocity),
      frameSPtr_(aReferenceFrame)
{
}

bool State::operator==(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false;
    }

    return (this->instant_ == aState.instant_) && (this->position_ == aState.position_) &&
           (this->velocity_ == aState.velocity_) && (this->attitude_ == aState.attitude_) &&
           (this->angularVelocity_ == aState.angularVelocity_) && (this->frameSPtr_ == aState.frameSPtr_);
}

bool State::operator!=(const State& aState) const
{
    return !((*this) == aState);
}

std::ostream& operator<<(std::ostream& anOutputStream, const State& aState)
{
    aState.print(anOutputStream);

    return anOutputStream;
}

bool State::isDefined() const
{
    return this->instant_.isDefined() && this->position_.isDefined() && this->velocity_.isDefined() &&
           this->attitude_.isDefined() && this->angularVelocity_.isDefined() && (this->frameSPtr_ != nullptr) &&
           this->frameSPtr_->isDefined();
}

const Instant& State::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->instant_;
}

const Position& State::accessPosition() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->position_;
}

const Velocity& State::accessVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->velocity_;
}

const Quaternion& State::accessAttitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->attitude_;
}

const Vector3d& State::accessAngularVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->angularVelocity_;
}

Instant State::getInstant() const
{
    return this->accessInstant();
}

Position State::getPosition() const
{
    return this->accessPosition();
}

Velocity State::getVelocity() const
{
    return this->accessVelocity();
}

Quaternion State::getAttitude() const
{
    return this->accessAttitude();
}

Vector3d State::getAngularVelocity() const
{
    return this->accessAngularVelocity();
}

VectorXd                        State::getCoordinates                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    VectorXd coordinates(12) ;
    coordinates.segment(0, 3) = this->accessPosition().accessCoordinates() ;
    coordinates.segment(3, 3) = this->accessVelocity().accessCoordinates() ;
    coordinates.segment(6, 4) = this->accessAttitude().toVector() ;
    coordinates.segment(10, 3) = this->accessAngularVelocity() ;

    return coordinates ;

}

Shared<const Frame>             State::getFrame                             ( ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->frameSPtr_;
}

State State::inFrame(const Shared<const Frame>& aFrameSPtr) const
{
    using ostk::physics::coord::Transform;

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    const Position position = this->position_.inFrame(aFrameSPtr, this->instant_);
    const Velocity velocity = this->velocity_.inFrame(this->position_, aFrameSPtr, this->instant_);

    const Quaternion& q_B_OLD = this->attitude_;
    const Vector3d& w_B_OLD_in_B = this->angularVelocity_;

    const Transform transform_NEW_OLD = this->frameSPtr_->getTransformTo(aFrameSPtr, this->instant_);

    const Quaternion q_NEW_OLD = transform_NEW_OLD.getOrientation();
    const Vector3d w_NEW_OLD_in_NEW = transform_NEW_OLD.getAngularVelocity();

    // q_B_NEW = q_B_OLD * q_OLD_NEW

    const Quaternion q_B_NEW = q_B_OLD * q_NEW_OLD.toConjugate();

    // w_B_NEW_in_B = w_B_OLD_in_B + w_OLD_NEW_in_B
    //              = w_B_OLD_in_B - q_B_NEW * w_NEW_OLD_in_NEW

    const Vector3d w_B_NEW_in_B = w_B_OLD_in_B - q_B_NEW * w_NEW_OLD_in_NEW;

    return {this->instant_, position, velocity, q_B_NEW, w_B_NEW_in_B, aFrameSPtr};
}

void State::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Flight :: Profile :: State") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Position:" << (this->position_.isDefined() ? this->position_.toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Velocity:" << (this->velocity_.isDefined() ? this->velocity_.toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Attitude:" << (this->attitude_.isDefined() ? this->attitude_.toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Angular velocity:"
        << (this->angularVelocity_.isDefined() ? this->angularVelocity_.toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream
    ) << "Frame:"
      << (((this->frameSPtr_ != nullptr) && this->frameSPtr_->isDefined()) ? this->frameSPtr_->getName() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

State State::Undefined()
{
    return {
        Instant::Undefined(),
        Position::Undefined(),
        Velocity::Undefined(),
        Quaternion::Undefined(),
        Vector3d::Undefined(),
        Frame::Undefined()};
}

}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk
