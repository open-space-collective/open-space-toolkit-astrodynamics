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

bool Conjunctive::isSatisfied(const Real& currentValue, const Real& previousValue) const
{
    for (const Shared<EventCondition>& eventCondition : eventConditions_)
    {
        if (!eventCondition->isSatisfied(currentValue, previousValue))
        {
            return false;
        }
    }

    return true;
}

bool Conjunctive::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    for (const Shared<EventCondition>& eventCondition : eventConditions_)
    {
        if (!eventCondition->isSatisfied(currentStateVector, currentTime, previousStateVector, previousTime))
        {
            return false;
        }
    }

    return true;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
