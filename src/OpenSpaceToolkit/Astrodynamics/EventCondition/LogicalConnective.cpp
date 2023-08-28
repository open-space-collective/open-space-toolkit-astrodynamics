/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{
LogicalConnective::LogicalConnective(const String& aName, const Array<Shared<EventCondition>>& eventConditions)
    : EventCondition(aName, EventCondition::Criteria::Undefined),
      eventConditions_(eventConditions)
{
}

LogicalConnective::~LogicalConnective() {}

Array<Shared<EventCondition>> LogicalConnective::getEventConditions() const
{
    return eventConditions_;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
