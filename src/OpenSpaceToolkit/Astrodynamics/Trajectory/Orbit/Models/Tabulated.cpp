// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Integer& anInitialRevolutionNumber,
    const InterpolationType& anInterpolationType
)
    : trajectory::orbit::Model(),
      trajectory::models::Tabulated(aStateArray, anInterpolationType),
      initialRevolutionNumber_(anInitialRevolutionNumber)
{
}

Tabulated* Tabulated::clone() const
{
    return new Tabulated(*this);
}

bool Tabulated::operator==(const Tabulated& aTabulatedModel) const
{
    if ((!this->isDefined()) || (!aTabulatedModel.isDefined()))
    {
        return false;
    }

    return (trajectory::models::Tabulated::operator==(aTabulatedModel)) &&
           (initialRevolutionNumber_ == aTabulatedModel.initialRevolutionNumber_);
}

bool Tabulated::operator!=(const Tabulated& aTabulatedModel) const
{
    return !((*this) == aTabulatedModel);
}

bool Tabulated::isDefined() const
{
    return trajectory::models::Tabulated::isDefined() && initialRevolutionNumber_.isDefined();
}

Instant Tabulated::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return this->getInterval().accessStart();
}

Integer Tabulated::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return initialRevolutionNumber_;
}

State Tabulated::calculateStateAt(const Instant& anInstant) const
{
    return trajectory::models::Tabulated::calculateStateAt(anInstant);
}

Integer Tabulated::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    throw ostk::core::error::runtime::ToBeImplemented("Kepler::calculateRevolutionNumberAt");

    // [TBI]

    return Integer::Undefined();
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    trajectory::models::Tabulated::print(anOutputStream, displayDecorator);
}

bool Tabulated::operator==(const trajectory::Model& aModel) const
{
    const Tabulated* tabulatedModelPtr = dynamic_cast<const Tabulated*>(&aModel);

    return (tabulatedModelPtr != nullptr) && this->operator==(*tabulatedModelPtr);
}

bool Tabulated::operator!=(const trajectory::Model& aModel) const
{
    return !((*this) == aModel);
}

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
