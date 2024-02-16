/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace system
{

using ostk::physics::environment::gravitational::Earth;

PropulsionSystem::PropulsionSystem(const Real& aThrustInSIUnit, const Real& aSpecificImpulseInSIUnit)
    : thrust_(aThrustInSIUnit),
      specificImpulse_(aSpecificImpulseInSIUnit),
      massFlowRate_(Real::Undefined())
{
    if (aThrustInSIUnit.isDefined() && aSpecificImpulseInSIUnit.isDefined())
    {
        massFlowRate_ = {aThrustInSIUnit / (aSpecificImpulseInSIUnit * Earth::gravityConstant)};
    }
}

bool PropulsionSystem::operator==(const PropulsionSystem& aPropulsionSystem) const
{
    if ((!this->isDefined()) || (!aPropulsionSystem.isDefined()))
    {
        return false;
    }

    return (thrust_ == aPropulsionSystem.thrust_) && (specificImpulse_ == aPropulsionSystem.specificImpulse_) &&
           (massFlowRate_ == aPropulsionSystem.massFlowRate_);
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
    return thrust_.isDefined() && specificImpulse_.isDefined() && massFlowRate_.isDefined();
}

void PropulsionSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Thrust:" << (thrust_.isDefined() ? thrust_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Specific Impulse:" << (specificImpulse_.isDefined() ? specificImpulse_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mass Flow Rate:" << (massFlowRate_.isDefined() ? massFlowRate_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real PropulsionSystem::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return thrust_;
}

Real PropulsionSystem::getSpecificImpulse() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return specificImpulse_;
}

Real PropulsionSystem::getMassFlowRate() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return massFlowRate_;
}

Real PropulsionSystem::getAcceleration(const Mass& aMass) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return {thrust_ / aMass.inKilograms()};
}

PropulsionSystem PropulsionSystem::Undefined()
{
    return {
        Real::Undefined(),
        Real::Undefined(),
    };
}

PropulsionSystem PropulsionSystem::Default()
{
    return {
        1.0,
        1000.0,
    };
}

}  // namespace system
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
