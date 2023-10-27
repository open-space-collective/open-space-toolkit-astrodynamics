/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::physics::time::Duration;

InstantCondition::InstantCondition(const Criterion& aCriterion, const Instant& anInstant)
    : RealCondition(
          "Instant Condition",
          aCriterion,
          [](const State& aState) -> Real
          {
              return (aState.accessInstant() - Instant::J2000()).inSeconds();
          },
          (anInstant - Instant::J2000()).inSeconds()
      )
{
}

InstantCondition::~InstantCondition() {}

Instant InstantCondition::getInstant() const
{
    return Instant::J2000() + Duration::Seconds(getTarget().value_);
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
