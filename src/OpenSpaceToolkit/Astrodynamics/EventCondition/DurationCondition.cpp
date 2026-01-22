/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

DurationCondition::DurationCondition(
    const Criterion& aCriterion, const Duration& aDuration, const Instant& aReferenceInstant
)
    : DurationCondition(aCriterion, aDuration, std::make_shared<Instant>(aReferenceInstant))
{
}

DurationCondition::DurationCondition(
    const Criterion& aCriterion, const Duration& aDuration, const Shared<Instant>& aReferenceInstantSPtr
)
    : RealCondition(
          "Duration Condition",
          aCriterion,
          [referenceInstantSPtr = aReferenceInstantSPtr](const State& aState) -> Real
          {
              return (aState.accessInstant() - *referenceInstantSPtr).inSeconds();
          },
          aDuration.inSeconds()
      ),
      referenceInstantSPtr_(aReferenceInstantSPtr)
{
}

DurationCondition::~DurationCondition() {}

Duration DurationCondition::getDuration() const
{
    return Duration::Seconds(target_.value);
}

Instant DurationCondition::getReferenceInstant() const
{
    return *referenceInstantSPtr_;
}

void DurationCondition::updateTarget(const State& aState)
{
    *referenceInstantSPtr_ = aState.accessInstant();
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
