/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

RealEventCondition::RealEventCondition(
    const String& aName,
    const Criteria& aCriteria,
    const std::function<Real(const State& aState)> anEvaluator,
    const Real& aTarget
)
    : EventCondition(aName, aCriteria),
      evaluator_(anEvaluator),
      target_(aTarget)
{
}

RealEventCondition::~RealEventCondition() {}

Real RealEventCondition::getTarget() const
{
    return target_;
}

Real RealEventCondition::evaluate(const State& aState) const
{
    return this->evaluator_(aState) - target_;
}

bool RealEventCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    const Real currentValue = evaluate(currentState);
    const Real previousValue = evaluate(previousState);

    return getComparator()(currentValue, previousValue);
}

void RealEventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Real Event Condition") : void();

    EventCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "Target:" << getTarget();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
