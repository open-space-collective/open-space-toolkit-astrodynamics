/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

InstantCondition::InstantCondition(const Criterion& aCriterion, const Instant& anInstant)
    : RealCondition(
          "Instant Condition",
          aCriterion,
          [anInstant](const State& aState) -> Real
          {
              return (aState.getInstant() - anInstant).inSeconds();
          },
          0.0
      ),
      instant_(anInstant)
{
}

InstantCondition::~InstantCondition() {}

Instant InstantCondition::getInstant() const
{
    return instant_;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
