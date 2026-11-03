/// Apache License 2.0

#include <cmath>
#include <limits>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Utility/String.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace system
{

using ostk::physics::environment::gravitational::Earth;

PropulsionSystem::PropulsionSystem(double aThrustInSIUnit, double aSpecificImpulseInSIUnit)
    : thrust_(aThrustInSIUnit),
      specificImpulse_(aSpecificImpulseInSIUnit),
      massFlowRate_(std::numeric_limits<double>::quiet_NaN())
{
    if (!std::isnan(aThrustInSIUnit) && !std::isnan(aSpecificImpulseInSIUnit))
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
    return !std::isnan(thrust_) && !std::isnan(specificImpulse_) && !std::isnan(massFlowRate_);
}

void PropulsionSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Thrust:" << (!std::isnan(thrust_) ? utility::DoubleToString(thrust_) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Specific Impulse:"
        << (!std::isnan(specificImpulse_) ? utility::DoubleToString(specificImpulse_) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mass Flow Rate:" << (!std::isnan(massFlowRate_) ? utility::DoubleToString(massFlowRate_) : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

double PropulsionSystem::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return thrust_;
}

double PropulsionSystem::getSpecificImpulse() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return specificImpulse_;
}

double PropulsionSystem::getMassFlowRate() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return massFlowRate_;
}

double PropulsionSystem::getAcceleration(const Mass& aMass) const
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
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::quiet_NaN(),
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
