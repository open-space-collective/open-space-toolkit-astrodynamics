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
    const std::function<Real(const VectorXd&, const Real&)> anEvaluator,
    const Real& aTarget
)
    : EventCondition(aName),
      criterion_(aCriterion),
      evaluator_(anEvaluator),
      target_(aTarget),
      comparator_(GenerateComparator(aCriterion))
{
}

RealCondition::~RealCondition() {}

RealCondition::Criterion RealCondition::getCriterion() const
{
    return criterion_;
}

std::function<Real(const VectorXd&, const Real&)> RealCondition::getEvaluator() const
{
    return evaluator_;
}

Real RealCondition::getTarget() const
{
    return target_;
}

void RealCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    EventCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << getName();
    ostk::core::utils::Print::Line(anOutputStream) << "Criterion:" << StringFromCriterion(getCriterion());
    ostk::core::utils::Print::Line(anOutputStream) << "Target:" << getTarget();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real RealCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    return this->evaluator_(aStateVector, aTime) - target_;
}

bool RealCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    return comparator_(evaluate(currentStateVector, currentTime), evaluate(previousStateVector, previousTime));
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
