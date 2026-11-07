/// Apache License 2.0

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
      massFlowRate_(aThrustInSIUnit / (aSpecificImpulseInSIUnit * Earth::gravityConstant))
{
}

bool PropulsionSystem::operator==(const PropulsionSystem& aPropulsionSystem) const
{
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

void PropulsionSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Thrust:" << utility::DoubleToString(thrust_);
    ostk::core::utils::Print::Line(anOutputStream) << "Specific Impulse:" << utility::DoubleToString(specificImpulse_);
    ostk::core::utils::Print::Line(anOutputStream) << "Mass Flow Rate:" << utility::DoubleToString(massFlowRate_);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

double PropulsionSystem::getThrust() const
{
    return thrust_;
}

double PropulsionSystem::getSpecificImpulse() const
{
    return specificImpulse_;
}

double PropulsionSystem::getMassFlowRate() const
{
    return massFlowRate_;
}

double PropulsionSystem::getAcceleration(const Mass& aMass) const
{
    return {thrust_ / aMass.inKilograms()};
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
