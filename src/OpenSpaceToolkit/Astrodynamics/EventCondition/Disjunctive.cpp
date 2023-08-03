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

bool Disjunctive::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
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
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
