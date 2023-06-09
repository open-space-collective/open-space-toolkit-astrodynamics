/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionModel.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

PropulsionModel::PropulsionModel(const Scalar& aThrust, const Scalar& aSpecificImpulse)
    : thrust_(aThrust),
      specificImpulse_(aSpecificImpulse)
{
}

PropulsionModel::PropulsionModel(const Real& aThrust, const Real& aSpecificImpulse)
    : thrust_({aThrust, thrustSIUnit_}),
      specificImpulse_({aSpecificImpulse, specificImpulseSIUnit_})
{
}

bool PropulsionModel::operator==(const PropulsionModel& aPropulsionModel) const
{
    if ((!this->isDefined()) || (!aPropulsionModel.isDefined()))
    {
        return false;
    }

    return (thrust_ == aPropulsionModel.thrust_) && (specificImpulse_ == aPropulsionModel.specificImpulse_);
}

bool PropulsionModel::operator!=(const PropulsionModel& aPropulsionModel) const
{
    return !((*this) == aPropulsionModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const PropulsionModel& aPropulsionModel)
{
    aPropulsionModel.print(anOutputStream);

    return anOutputStream;
}

bool PropulsionModel::isDefined() const
{
    return thrust_.isDefined() && specificImpulse_.isDefined();
}

void PropulsionModel::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

    anOutputStream << "[Thrust = " << thrust_ << ", Specific Impulse = " << specificImpulse_
                   << ", Mass Flow Rate = " << getMassFlowRate() << "]";

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Scalar PropulsionModel::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionModel");
    }

    return thrust_;
}

Scalar PropulsionModel::getSpecificImpulse() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionModel");
    }

    return specificImpulse_;
}

Scalar PropulsionModel::getMassFlowRate() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionModel");
    }

    return {
        thrust_.getValue() / (specificImpulse_.getValue() * 9.80665),
        massFlowRateSIUnit_};  // TBM: Replace with gravity constant
}

Real PropulsionModel::getAcceleration(const Mass& aMass) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionModel");
    }

    return thrust_.getValue() / aMass.inKilograms();
}

PropulsionModel PropulsionModel::Undefined()
{
    return {Scalar::Undefined(), Scalar::Undefined()};
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
