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
    const Criteria& aCriteria,
    const std::function<Real(const VectorXd&, const Real&)> anEvaluator,
    const Real& aTarget
)
    : EventCondition(aName),
      criteria_(aCriteria),
      evaluator_(anEvaluator),
      target_(aTarget),
      comparator_(GenerateComparator(aCriteria))
{
}

RealCondition::~RealCondition() {}

RealCondition::Criteria RealCondition::getCriteria() const
{
    return criteria_;
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
    ostk::core::utils::Print::Line(anOutputStream) << "Criteria:" << StringFromCriteria(getCriteria());
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

String RealCondition::StringFromCriteria(const Criteria& aCriteria)
{
    switch (aCriteria)
    {
        case Criteria::PositiveCrossing:
            return "Positive Crossing";

        case Criteria::NegativeCrossing:
            return "Negative Crossing";

        case Criteria::AnyCrossing:
            return "Any Crossing";

        case Criteria::StrictlyPositive:
            return "Strictly Positive";

        case Criteria::StrictlyNegative:
            return "Strictly Negative";

        default:
            throw ostk::core::error::runtime::Wrong("Criteria");
    }

    return String::Empty();
}

std::function<bool(const Real&, const Real&)> RealCondition::GenerateComparator(const RealCondition::Criteria& aCriteria
)
{
    switch (aCriteria)
    {
        case RealCondition::Criteria::StrictlyPositive:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue > 0.0);
            };

        case RealCondition::Criteria::StrictlyNegative:
            return [](const Real& currentValue, [[maybe_unused]] const Real& previousValue) -> bool
            {
                return (currentValue < 0.0);
            };

        case RealCondition::Criteria::PositiveCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue < 0.0) && (currentValue > 0.0);
            };

        case RealCondition::Criteria::NegativeCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (previousValue > 0.0) && (currentValue < 0.0);
            };

        case RealCondition::Criteria::AnyCrossing:
            return [](const Real& currentValue, const Real& previousValue) -> bool
            {
                return (currentValue > 0.0) == (previousValue < 0.0);
            };

        default:
            throw ostk::core::error::runtime::Wrong("Criteria");
    }
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
