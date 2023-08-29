/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

DurationCondition::DurationCondition(const Criteria& aCriteria, const Instant& startInstant, const Duration& aDuration)
    : RealEventCondition(
          "Duration Condition",
          aCriteria,
          [startInstant]([[maybe_unused]] const State& aState) -> Real
          {
              return (aState.accessInstant() - startInstant).inSeconds();
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
