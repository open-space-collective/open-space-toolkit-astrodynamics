/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::physics::time::Duration;

InstantCondition::InstantCondition(const Criterion& aCriterion, const Instant& anInstant)
    : RealCondition(
          "Instant Condition",
          aCriterion,
          [anInstant](const State& aState) -> double
          {
              return (aState.getInstant() - anInstant).inSeconds();
          },
          0.0
      ),
      instant_(anInstant)
{
}

InstantCondition::~InstantCondition() {}

const Instant& InstantCondition::getInstant() const
{
    return instant_;
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
