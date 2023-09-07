/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

AngularCondition::AngularCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)>& anEvaluator,
    const Angle& aTargetAngle
)
    : EventCondition(aName),
      criterion_(aCriterion),
      evaluator_(anEvaluator),
      comparator_(GenerateComparator(aCriterion, aTargetAngle.inRadians())),
      target_(aTargetAngle.inRadians()),
      targetRange_(std::make_pair(Real::Undefined(), Real::Undefined()))
{
}

AngularCondition::~AngularCondition() {}

AngularCondition::Criterion AngularCondition::getCriterion() const
{
    return criterion_;
}

std::function<Real(const State&)> AngularCondition::getEvaluator() const
{
    return evaluator_;
}

Angle AngularCondition::getTargetAngle() const
{
    if (!target_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Target");
    }

    return Angle::Radians(target_);
}

Pair<Angle, Angle> AngularCondition::getTargetRange() const
{
    if (!targetRange_.first.isDefined() || !targetRange_.second.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Target Range");
    }

    return Pair<Angle, Angle>(Angle::Radians(targetRange_.first), Angle::Radians(targetRange_.second));
}

void AngularCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    EventCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "Criterion: " << StringFromCriterion(getCriterion());
    if (target_.isDefined())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Target: " << getTargetAngle();
    }
    else
    {
        ostk::core::utils::Print::Line(anOutputStream)
            << "Target Range: " << getTargetRange().first << " - " << getTargetRange().second;
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool AngularCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return comparator_(evaluator_(currentState), evaluator_(previousState));
}

AngularCondition AngularCondition::WithinRange(
    const String& aName, const std::function<Real(const State&)>& anEvaluator, const Pair<Angle, Angle>& aTargetRange
)
{
    return AngularCondition(aName, Criterion::WithinRange, anEvaluator, aTargetRange);
}

String AngularCondition::StringFromCriterion(const Criterion& aCriterion)
{
    switch (aCriterion)
    {
        case Criterion::PositiveCrossing:
            return "Positive Crossing";
        case Criterion::NegativeCrossing:
            return "Negative Crossing";
        case Criterion::AnyCrossing:
            return "Any Crossing";
        case Criterion::WithinRange:
            return "Within Range";
        default:
            throw ostk::core::error::runtime::Wrong("Criterion");
    }
}

bool AngularCondition::IsPositiveCrossing(const Real& currentValue, const Real& previousValue, const Real& targetValue)
{
    // Calculate angular differences
    const Real deltaAngle = std::fmod(currentValue - previousValue + 540.0, 360.0) - 180.0;

    if (deltaAngle <= 0.0)
    {
        return false;
    }

    if (previousValue < targetValue && currentValue >= targetValue)
    {
        return true;
    }

    return (currentValue < previousValue) && (targetValue >= previousValue || targetValue < currentValue);
}

bool AngularCondition::IsNegativeCrossing(const Real& currentValue, const Real& previousValue, const Real& targetValue)
{
    // Calculate angular differences
    const Real deltaAngle = std::fmod(currentValue - previousValue + 540.0, 360.0) - 180.0;

    if (deltaAngle >= 0.0)
    {
        return false;
    }

    if (previousValue > targetValue && currentValue <= targetValue)
    {
        return true;
    }

    return (currentValue > previousValue) && (targetValue <= previousValue || targetValue > currentValue);
}

std::function<bool(const Real&, const Real&)> AngularCondition::GenerateComparator(
    const AngularCondition::Criterion& aCriterion, const Real& aTarget
)
{
    switch (aCriterion)
    {
        case Criterion::PositiveCrossing:
            return [aTarget](const Real& currentValue, const Real& previousValue) -> bool
            {
                return AngularCondition::IsPositiveCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::NegativeCrossing:
            return [aTarget](const Real& currentValue, const Real& previousValue) -> bool
            {
                return AngularCondition::IsNegativeCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::AnyCrossing:
            return [aTarget](const Real& currentValue, const Real& previousValue) -> bool
            {
                return AngularCondition::IsPositiveCrossing(currentValue, previousValue, aTarget) ||
                       AngularCondition::IsNegativeCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::WithinRange:
            throw ostk::core::error::RuntimeError(
                "Cannot generate comparator for criterion 'WithinRange' with a single target value."
            );
        default:
            throw ostk::core::error::runtime::Wrong("Criterion");
    }
}

AngularCondition::AngularCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)>& anEvaluator,
    const Pair<Angle, Angle>& aTargetRange
)
    : EventCondition(aName),
      criterion_(aCriterion),
      evaluator_(anEvaluator),
      comparator_(
          [lowerBound = aTargetRange.first.inRadians(), upperBound = aTargetRange.second.inRadians()](
              const Real& currentValue, [[maybe_unused]] const Real& previousValue
          ) -> bool
          {
              return (currentValue >= lowerBound) && (currentValue <= upperBound);
          }
      ),
      target_(Real::Undefined()),
      targetRange_(std::make_pair(aTargetRange.first.inRadians(), aTargetRange.second.inRadians()))
{
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
