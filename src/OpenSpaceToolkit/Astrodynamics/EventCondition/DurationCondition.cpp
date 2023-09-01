/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

DurationCondition::DurationCondition(const Criterion& aCriterion, const Duration& aDuration)
    : RealCondition(
          "Duration Condition",
          aCriterion,
          []([[maybe_unused]] const VectorXd& aStateVector, const Real& aTime) -> Real
          {
              return aTime;
          },
          aDuration.inSeconds()
      )
{
}

DurationCondition::~DurationCondition() {}

Duration DurationCondition::getDuration() const
{
    return Duration::Seconds(getTarget());
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
