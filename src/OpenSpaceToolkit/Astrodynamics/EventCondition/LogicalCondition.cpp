/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

LogicalCondition::LogicalCondition(
    const String& aName,
    const LogicalCondition::Connective& aConnective,
    const Array<Shared<EventCondition>>& eventConditions)
    : EventCondition(aName),
      connective_(aConnective),
      eventConditions_(eventConditions)
{
}

LogicalCondition::~LogicalCondition() {}

LogicalCondition::Connective LogicalCondition::getConnective() const {
    return connective_;
}

Array<Shared<EventCondition>> LogicalCondition::getEventConditions() const
{
    return eventConditions_;
}

bool LogicalCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    switch (connective_)
    {
        case Connective::Conjunction:
            return std::all_of(
                eventConditions_.begin(),
                eventConditions_.end(),
                [&currentStateVector, &currentTime, &previousStateVector, &previousTime](
                    const Shared<EventCondition>& eventCondition
                ) -> bool
                {
                    return eventCondition->isSatisfied(currentStateVector, currentTime, previousStateVector, previousTime);
                }
            );
        
        case Connective::Disjunction:
            return std::any_of(
                eventConditions_.begin(),
                eventConditions_.end(),
                [&currentStateVector, &currentTime, &previousStateVector, &previousTime](
                    const Shared<EventCondition>& eventCondition
                ) -> bool
                {
                    return eventCondition->isSatisfied(currentStateVector, currentTime, previousStateVector, previousTime);
                }
            );
        
        case Connective::Undefined:
            throw ostk::core::error::runtime::Undefined("Connective");
        
        default:
            throw ostk::core::error::runtime::Wrong("Connective");
    }
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
