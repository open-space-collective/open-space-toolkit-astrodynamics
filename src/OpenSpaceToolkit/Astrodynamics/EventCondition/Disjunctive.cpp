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

// bool Disjunctive::isSatisfiedFromCache(const VectorXd& aStateVector, const Real& aTime)
// {
//     for (const Shared<EventCondition>& eventCondition : eventConditions_)
//     {
//         if (eventCondition->isSatisfiedFromCache(aStateVector, aTime))
//         {
//             return true;
//         }
//     }

//     return false;
// }

bool Disjunctive::isSatisfied(const Real& currentValue, const Real& previousValue) const
{
    for (const Shared<EventCondition>& eventCondition : eventConditions_)
    {
        if (eventCondition->isSatisfied(currentValue, previousValue))
        {
            return true;
        }
    }

    return false;
}

// bool Disjunctive::isSatisfied(
//     const VectorXd& currentStateVector,
//     const Real& currentTime,
//     const VectorXd& previousStateVector,
//     const Real& previousTime
// ) const
// {
//     for (const Shared<EventCondition>& eventCondition : eventConditions_)
//     {
//         if (eventCondition->isSatisfied(currentStateVector, currentTime, previousStateVector, previousTime))
//         {
//             return true;
//         }
//     }

//     return false;
// }

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk