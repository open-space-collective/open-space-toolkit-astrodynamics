/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Maneuver.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

Maneuver::Maneuver(const Instant& anInstant, const Duration& aDuration, const Velocity& aVelocityDelta)
    : instant_(anInstant),
      duration_(aDuration),
      velocityDelta_(aVelocityDelta)
{
}

bool Maneuver::operator==(const Maneuver& aManeuver) const
{
    if ((!this->isDefined()) || (!aManeuver.isDefined()))
    {
        return false;
    }

    return (this->instant_ == aManeuver.instant_) && (this->duration_ == aManeuver.duration_) &&
           (this->velocityDelta_ == aManeuver.velocityDelta_);
}

bool Maneuver::operator!=(const Maneuver& aManeuver) const
{
    return !((*this) == aManeuver);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Maneuver& aManeuver)
{
    aManeuver.print(anOutputStream);

    return anOutputStream;
}

bool Maneuver::isDefined() const
{
    return this->instant_.isDefined() && this->duration_.isDefined() && this->velocityDelta_.isDefined();
}

const Instant& Maneuver::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Maneuver");
    }

    return this->instant_;
}

const Duration& Maneuver::accessDuration() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Maneuver");
    }

    return this->duration_;
}

const Velocity& Maneuver::accessVelocityDelta() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Maneuver");
    }

    return this->velocityDelta_;
}

Instant Maneuver::getInstant() const
{
    return this->accessInstant();
}

Duration Maneuver::getDuration() const
{
    return this->accessDuration();
}

Velocity Maneuver::getVelocityDelta() const
{
    return this->accessVelocityDelta();
}

// Maneuver Maneuver::inFrame(const Shared<const Frame>& aFrameSPtr) const
// {
//     if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
//     {
//         throw ostk::core::error::runtime::Undefined("Frame");
//     }

//     if (!this->isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("Maneuver");
//     }

//     const Velocity velocityDelta = velocityDelta_.inFrame(aFrameSPtr, this->instant_);

//     return {this->instant_, this->duration_, velocityDelta};
// }

void Maneuver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: Maneuver") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Duration:" << (this->duration_.isDefined() ? this->duration_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Velocity:" << (this->velocityDelta_.isDefined() ? this->velocityDelta_.toString(12) : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Maneuver Maneuver::Undefined()
{
    return {Instant::Undefined(), Duration::Undefined(), Velocity::Undefined()};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
