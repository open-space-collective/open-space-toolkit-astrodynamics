/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanEventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

BooleanEventCondition::BooleanEventCondition(const String& aName, const Criteria& aCriteria, const bool& anInverseFlag)
    : EventCondition(aName, aCriteria),
      inverse_(anInverseFlag)
{
}

BooleanEventCondition::~BooleanEventCondition() {}

Real BooleanEventCondition::isInversed() const
{
    return inverse_;
}

bool BooleanEventCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    const Real currentValue = compute(currentStateVector, currentTime) != inverse_ ? 1.0 : -1.0;
    const Real previousValue = compute(previousStateVector, previousTime) != inverse_ ? 1.0 : -1.0;

    return getComparator()(currentValue, previousValue);
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
