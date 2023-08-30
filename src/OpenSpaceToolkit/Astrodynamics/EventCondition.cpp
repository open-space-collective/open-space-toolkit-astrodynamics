/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{

EventCondition::EventCondition(
    const String& aName,
    const Criteria& aCriteria,
    const std::function<Real(const VectorXd&, const Real&)> anEvaluator,
    const Real& aTarget)
    : name_(aName),
      criteria_(aCriteria),
      evaluator_(anEvaluator),
      target_(aTarget),
      comparator_(getComparator(aCriteria))
{
}

EventCondition::~EventCondition() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition)
{
    anEventCondition.print(anOutputStream);

    return anOutputStream;
}

String EventCondition::getName() const
{
    return name_;
}

EventCondition::Criteria EventCondition::getCriteria() const
{
    return criteria_;
}

std::function<bool(const Real&, const Real&)> EventCondition::getComparator() const
{
    return comparator_;
}

Real RealEventCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    return this->evaluator_(aStateVector, aTime) - this->target_;
}

bool EventCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    const Real currentValue = evaluate(currentStateVector, currentTime);
    const Real previousValue = evaluate(previousStateVector, previousTime);

    this->comparator_(currentValue, previousValue);
}

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << getName();
    ostk::core::utils::Print::Line(anOutputStream) << "Criteria:" << StringFromCriteria(getCriteria());

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

String EventCondition::StringFromCriteria(const Criteria& aCriteria)
{
    switch (aCriteria)
    {
        case Criteria::PositiveCrossing:
            return "Positive Crossing";

        case Criteria::NegativeCrossing:
            return "Negative Crossing";

        case Criteria::AnyCrossing:
            return "Any Crossing";

        case Criteria::StrictlyPositive:
            return "Strictly Positive";

        case Criteria::StrictlyNegative:
            return "Strictly Negative";

        case Criteria::Undefined:
            return "Undefined";

        default:
            throw ostk::core::error::runtime::Wrong("Criteria");
    }

    return String::Empty();
}

std::function<bool(const Real&, const Real&)> EventCondition::getComparator(const EventCondition::Criteria& aCriteria)
{
    switch (aCriteria)
    {
        case EventCondition::Criteria::StrictlyPositive:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue > 0.0);
            };

        case EventCondition::Criteria::StrictlyNegative:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue < 0.0);
            };

        case EventCondition::Criteria::PositiveCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue < 0.0) && (currentValue > 0.0);
            };

        case EventCondition::Criteria::NegativeCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue > 0.0) && (currentValue < 0.0);
            };

        case EventCondition::Criteria::AnyCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (currentValue > 0.0) == (previousValue < 0.0);
            };

        case EventCondition::Criteria::Undefined:
            return []([[maybe_unused]] const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                throw ostk::core::error::runtime::Undefined("Comparator");

                return false;
            };

        default:
            throw ostk::core::error::runtime::Wrong("Criteria");
    }
}

}  // namespace astro
}  // namespace ostk
