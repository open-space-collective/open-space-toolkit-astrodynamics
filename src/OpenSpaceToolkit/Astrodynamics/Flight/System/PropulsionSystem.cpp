/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

PropulsionSystem::PropulsionSystem(const Scalar& aThrust, const Scalar& aSpecificImpulse)
    : thrust_(aThrust),
      specificImpulse_(aSpecificImpulse)
{
}

PropulsionSystem::PropulsionSystem(const Real& aThrust, const Real& aSpecificImpulse)
    : thrust_({aThrust, thrustSIUnit_}),
      specificImpulse_({aSpecificImpulse, specificImpulseSIUnit_})
{
}

bool PropulsionSystem::operator==(const PropulsionSystem& aPropulsionSystem) const
{
    if ((!this->isDefined()) || (!aPropulsionSystem.isDefined()))
    {
        return false;
    }

    return (thrust_ == aPropulsionSystem.thrust_) && (specificImpulse_ == aPropulsionSystem.specificImpulse_);
}

bool PropulsionSystem::operator!=(const PropulsionSystem& aPropulsionSystem) const
{
    return !((*this) == aPropulsionSystem);
}

std::ostream& operator<<(std::ostream& anOutputStream, const PropulsionSystem& aPropulsionSystem)
{
    aPropulsionSystem.print(anOutputStream);

    return anOutputStream;
}

bool PropulsionSystem::isDefined() const
{
    return thrust_.isDefined() && specificImpulse_.isDefined();
}

void PropulsionSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

    anOutputStream << "[Thrust = " << thrust_ << ", Specific Impulse = " << specificImpulse_
                   << ", Mass Flow Rate = " << getMassFlowRate() << "]";

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Scalar PropulsionSystem::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return thrust_;
}

Scalar PropulsionSystem::getSpecificImpulse() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return specificImpulse_;
}

Scalar PropulsionSystem::getMassFlowRate() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return {
        thrust_.getValue() / (specificImpulse_.getValue() * 9.80665),
        massFlowRateSIUnit_};  // TBM: Replace with gravity constant
}

Real PropulsionSystem::getAcceleration(const Mass& aMass) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return thrust_.getValue() / aMass.inKilograms();
}

PropulsionSystem PropulsionSystem::Undefined()
{
    return {Scalar::Undefined(), Scalar::Undefined()};
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
