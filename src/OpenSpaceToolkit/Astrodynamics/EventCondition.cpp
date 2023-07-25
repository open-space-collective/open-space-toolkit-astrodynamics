/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{

EventCondition::EventCondition(const String& aName, const Criteria& aCriteria)
    : name_(aName),
      criteria_(aCriteria),
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

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << getName();
    ostk::core::utils::Print::Line(anOutputStream) << "Criteria:" << StringFromCriteria(getCriteria());

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool EventCondition::isSatisfied(const Real& currentValue, const Real& previousValue) const
{
    return comparator_(currentValue, previousValue);
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
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                (void)previousValue;
                return (currentValue > 0.0);
            };

        case EventCondition::Criteria::StrictlyNegative:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                (void)previousValue;
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
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                (void)currentValue;
                (void)previousValue;

                throw ostk::core::error::runtime::Undefined("Comparator");

                return false;
            };

        default:
            throw ostk::core::error::runtime::Wrong("Criteria");
    }
}

}  // namespace astro
}  // namespace ostk
