/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{
LogicalConnective::LogicalConnective(const String& aName, const Array<Shared<EventCondition>>& eventConditions)
    : EventCondition(aName, EventCondition::Criteria::Undefined, Real::Undefined()),
      eventConditions_(eventConditions)
{
}

LogicalConnective::~LogicalConnective() {}

Array<Shared<EventCondition>> LogicalConnective::getEventConditions() const
{
    return eventConditions_;
}

bool LogicalConnective::isSatisfied(const Real& currentValue, const Real& previousValue) const
{
    (void)currentValue;
    (void)previousValue;

    throw ostk::core::error::runtime::Undefined("LogicalConnective::isSatisfied");
}

Real LogicalConnective::compute(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aStateVector;
    (void)aTime;

    throw ostk::core::error::runtime::Undefined("LogicalConnective::compute");
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
