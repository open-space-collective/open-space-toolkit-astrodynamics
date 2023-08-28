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
    const std::function<bool(const VectorXd&, const Real&)> anEvaluator,
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

bool BooleanEventCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    return inverse_ ? !this->evaluator_(aStateVector, aTime) : this->evaluator_(aStateVector, aTime);
}

bool BooleanEventCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    const Real currentValue = evaluate(currentStateVector, currentTime) ? 1.0 : -1.0;
    const Real previousValue = evaluate(previousStateVector, previousTime) ? 1.0 : -1.0;

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
