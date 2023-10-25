/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

RealCondition::RealCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)> anEvaluator,
    const Real& aTarget,
    const bool& targetIsRelative
)
    : EventCondition(aName, anEvaluator, aTarget, targetIsRelative),
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
    ostk::core::utils::Print::Line(anOutputStream) << "Target: " << getTarget();
    ostk::core::utils::Print::Line(anOutputStream) << "Target type: " << (targetIsRelative_ ? "Relative" : "Absolute");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real RealCondition::evaluate(const State& aState) const
{
    return this->evaluator_(aState) - target_;
}

bool RealCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return comparator_(evaluate(currentState), evaluate(previousState));
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
}  // namespace astro
}  // namespace ostk
