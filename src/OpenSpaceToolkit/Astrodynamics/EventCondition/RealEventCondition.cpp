/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

RealEventCondition::RealEventCondition(const String& aName, const Criteria& aCriteria, const Real& aTarget)
    : EventCondition(aName, aCriteria),
      target_(aTarget)
{
}

RealEventCondition::~RealEventCondition() {}

Real RealEventCondition::getTarget() const
{
    return target_;
}

Real RealEventCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    return compute(aStateVector, aTime) - target_;
}

bool RealEventCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    const Real currentValue = evaluate(currentStateVector, currentTime);
    const Real previousValue = evaluate(previousStateVector, previousTime);

    return getComparator()(currentValue, previousValue);
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
