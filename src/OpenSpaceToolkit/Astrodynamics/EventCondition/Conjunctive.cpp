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

bool Conjunctive::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
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
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
