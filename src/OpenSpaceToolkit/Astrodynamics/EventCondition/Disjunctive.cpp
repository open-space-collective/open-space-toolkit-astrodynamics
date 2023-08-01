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
    for (const Shared<EventCondition>& eventCondition : eventConditions_)
    {
        if (eventCondition->isSatisfied(currentStateVector, currentTime, previousStateVector, previousTime))
        {
            return true;
        }
    }

    return false;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
