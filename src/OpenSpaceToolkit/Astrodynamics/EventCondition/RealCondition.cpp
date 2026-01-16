/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::physics::time::Instant;

RealCondition::RealCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)>& anEvaluator,
    const Real& aTargetValue
)
    : EventCondition(aName, anEvaluator, aTargetValue),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion))
{
}

RealCondition::RealCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)>& anEvaluator,
    const Target& aTarget
)
    : EventCondition(aName, anEvaluator, aTarget),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion))
{
}

RealCondition::~RealCondition() {}

RealCondition::Criterion RealCondition::getCriterion() const
{
    return criterion_;
}

void RealCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    EventCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "Criterion: " << StringFromCriterion(getCriterion());

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real RealCondition::evaluate(const State& aState) const
{
    return this->evaluator_(aState) - (target_.value + target_.valueOffset);
}

bool RealCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return comparator_(evaluate(currentState), evaluate(previousState));
}

RealCondition* RealCondition::clone() const
{
    return new RealCondition(*this);
}

String RealCondition::StringFromCriterion(const Criterion& aCriterion)
{
    switch (aCriterion)
    {
        case Criterion::PositiveCrossing:
            return "Positive Crossing";

        case Criterion::NegativeCrossing:
            return "Negative Crossing";

        case Criterion::AnyCrossing:
            return "Any Crossing";

        case Criterion::StrictlyPositive:
            return "Strictly Positive";

        case Criterion::StrictlyNegative:
            return "Strictly Negative";

        default:
            throw ostk::core::error::runtime::Wrong("Criterion");
    }

    return String::Empty();
}

RealCondition RealCondition::DurationCondition(const Criterion& aCriterion, const Duration& aDuration)
{
    return {
        "Duration",
        aCriterion,
        [](const State& aState) -> Real
        {
            return (aState.accessInstant() - Instant::J2000()).inSeconds();
        },
        {aDuration.inSeconds(), EventCondition::Target::Type::Relative}
    };
}

std::function<bool(const Real&, const Real&)> RealCondition::GenerateComparator(
    const RealCondition::Criterion& aCriterion
)
{
    switch (aCriterion)
    {
        case RealCondition::Criterion::StrictlyPositive:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue > 0.0);
            };

        case RealCondition::Criterion::StrictlyNegative:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue < 0.0);
            };

        case RealCondition::Criterion::PositiveCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue < 0.0) && (currentValue > 0.0);
            };

        case RealCondition::Criterion::NegativeCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue > 0.0) && (currentValue < 0.0);
            };

        case RealCondition::Criterion::AnyCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (currentValue > 0.0) == (previousValue < 0.0);
            };

        default:
            throw ostk::core::error::runtime::Wrong("Criterion");
    }
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
