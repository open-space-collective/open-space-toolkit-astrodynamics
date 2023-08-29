/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Disjunctive.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{
Disjunctive::Disjunctive(const Array<Shared<EventCondition>>& eventConditions)
    : LogicalConnective("Disjunctive Condition", eventConditions)
{
}

Disjunctive::~Disjunctive() {}

bool Disjunctive::isSatisfied(const State& currentState, const State& previousState) const
{
    return std::any_of(
        eventConditions_.begin(),
        eventConditions_.end(),
        [&currentState, &previousState](const Shared<EventCondition>& eventCondition) -> bool
        {
            return eventCondition->isSatisfied(currentState, previousState);
        }
    );
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
