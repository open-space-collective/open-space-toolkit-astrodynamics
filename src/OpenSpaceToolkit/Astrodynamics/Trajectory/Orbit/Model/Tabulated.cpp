/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Integer& anInitialRevolutionNumber,
    const Interpolator::Type& anInterpolationType
)
    : trajectory::orbit::Model(),
      trajectory::model::Tabulated(aStateArray, anInterpolationType),
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

    return (trajectory::model::Tabulated::operator==(aTabulatedModel)) &&
           (initialRevolutionNumber_ == aTabulatedModel.initialRevolutionNumber_);
}

bool Tabulated::operator!=(const Tabulated& aTabulatedModel) const
{
    return !((*this) == aTabulatedModel);
}

bool Tabulated::isDefined() const
{
    return trajectory::model::Tabulated::isDefined() && initialRevolutionNumber_.isDefined();
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
    return trajectory::model::Tabulated::calculateStateAt(anInstant);
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    trajectory::model::Tabulated::print(anOutputStream, displayDecorator);
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

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
