/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

DurationCondition::DurationCondition(const Criteria& aCriteria, const Duration& aDuration)
    : EventCondition("Duration Condition", aCriteria),
      durationInSeconds_(aDuration.inSeconds())
{
}

DurationCondition::~DurationCondition() {}

Duration DurationCondition::getDuration() const
{
    return Duration::Seconds(durationInSeconds_);
}

Real DurationCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aStateVector;
    return aTime - durationInSeconds_;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
