/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

DurationCondition::DurationCondition(const Criteria& aCriteria, const Duration& aDuration)
    : RealEventCondition("Duration Condition", aCriteria, aDuration.inSeconds())
{
}

DurationCondition::~DurationCondition() {}

Duration DurationCondition::getDuration() const
{
    return Duration::Seconds(getTarget());
}

Real DurationCondition::compute(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aStateVector;
    return aTime;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
