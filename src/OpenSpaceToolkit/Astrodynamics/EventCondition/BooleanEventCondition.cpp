/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanEventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

BooleanEventCondition::BooleanEventCondition(
    const String& aName,
    const Criteria& aCriteria,
    const std::function<bool(const State&)> anEvaluator,
    const bool& anInverseFlag
)
    : EventCondition(aName, aCriteria),
      evaluator_(anEvaluator),
      inverse_(anInverseFlag)
{
}

BooleanEventCondition::~BooleanEventCondition() {}

bool BooleanEventCondition::isInversed() const
{
    return inverse_;
}

bool BooleanEventCondition::evaluate(const State& aState) const
{
    return inverse_ ? !this->evaluator_(aState) : this->evaluator_(aState);
}

bool BooleanEventCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    const Real currentValue = evaluate(currentState) ? 1.0 : -1.0;
    const Real previousValue = evaluate(previousState) ? 1.0 : -1.0;

    return getComparator()(currentValue, previousValue);
}

void BooleanEventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Boolean Event Condition") : void();

    EventCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "isInversed:" << (isInversed() ? "True" : "False");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
