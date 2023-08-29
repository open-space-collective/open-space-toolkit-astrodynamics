/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::physics::environment::gravitational::Earth;

ostk::physics::Unit PropulsionSystem::thrustSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Meter,
    {1},
    Mass::Unit::Kilogram,
    {1},
    Time::Unit::Second,
    {-2},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));

ostk::physics::Unit PropulsionSystem::specificImpulseSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Undefined,
    {0},
    Mass::Unit::Undefined,
    {0},
    Time::Unit::Second,
    {1},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));

ostk::physics::Unit PropulsionSystem::massFlowRateSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Undefined,
    {0},
    Mass::Unit::Kilogram,
    {1},
    Time::Unit::Second,
    {-1},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));

PropulsionSystem::PropulsionSystem(const Scalar& aThrust, const Scalar& aSpecificImpulse)
{
    if (aThrust.isDefined() && aSpecificImpulse.isDefined())
    {
        thrust_ = aThrust.inUnit(thrustSIUnit);
        specificImpulse_ = aSpecificImpulse.inUnit(specificImpulseSIUnit);

        massFlowRate_ = {
            aThrust.getValue() / (aSpecificImpulse.getValue() * Earth::gravityConstant), massFlowRateSIUnit
        };
    }

    else
    {
        thrust_ = aThrust;
        thrust_ = aSpecificImpulse;
        massFlowRate_ = Scalar::Undefined();
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

    anOutputStream << "[Thruster = " << thrust_ << ", Specific Impulse = " << specificImpulse_
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

    return massFlowRate_;
}

Scalar PropulsionSystem::getAcceleration(const Mass& aMass) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return {
        thrust_.getValue() / aMass.inKilograms(),
        Unit::Derived(Derived::Unit::Acceleration(Length::Unit::Meter, Time::Unit::Second)),
    };
}

PropulsionSystem PropulsionSystem::Undefined()
{
    return {
        Scalar(Real::Undefined(), thrustSIUnit),
        Scalar(Real::Undefined(), specificImpulseSIUnit),
    };
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
