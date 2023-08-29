/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

DurationCondition::DurationCondition(const Criteria& aCriteria, const Duration& aDuration)
    : RealEventCondition(
          "Duration Condition",
          aCriteria,
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
