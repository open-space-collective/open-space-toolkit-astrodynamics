/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Conjunctive.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{
Conjunctive::Conjunctive(const Array<Shared<EventCondition>>& eventConditions)
    : LogicalConnective("Conjunctive Condition", eventConditions)
{
}

Conjunctive::~Conjunctive() {}

bool Conjunctive::isSatisfied(const State& currentState, const State& previousState) const
{
    return std::all_of(
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
