/// Apache License 2.0

#include <cmath>
#include <limits>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

AngularCondition::AngularCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<double(const State&)>& anEvaluator,
    const Angle& aTargetAngle
)
    : EventCondition(aName, anEvaluator, aTargetAngle.inRadians(0.0, 2.0 * M_PI)),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion)),
      targetRange_(std::make_pair(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()))
{
}

AngularCondition::AngularCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<double(const State&)>& anEvaluator,
    const Target& aTarget
)
    : EventCondition(aName, anEvaluator, aTarget),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion)),
      targetRange_(std::make_pair(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()))
{
}

AngularCondition::~AngularCondition() {}

AngularCondition::Criterion AngularCondition::getCriterion() const
{
    return criterion_;
}

Angle AngularCondition::getTargetAngle() const
{
    if (std::isnan(target_.value))
    {
        throw ostk::core::error::runtime::Undefined("Target");
    }

    return Angle::Radians(target_.value);
}

Pair<Angle, Angle> AngularCondition::getTargetRange() const
{
    if (std::isnan(targetRange_.first) || std::isnan(targetRange_.second))
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
        ostk::core::utils::Print::Line(anOutputStream)
            << "Target:" << String::Format("{} [deg]", getTargetAngle().inDegrees());
    }
    else
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Target Range: \n"
                                                       << getTargetRange().first << "\n"
                                                       << getTargetRange().second;
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool AngularCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return comparator_(evaluator_(currentState), evaluator_(previousState), (target_.value + target_.valueOffset));
}

AngularCondition* AngularCondition::clone() const
{
    return new AngularCondition(*this);
}

AngularCondition AngularCondition::WithinRange(
    const String& aName, const std::function<double(const State&)>& anEvaluator, const Pair<Angle, Angle>& aTargetRange
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

bool AngularCondition::IsPositiveCrossing(double currentValue, double previousValue, double targetValue)
{
    // Calculate angular differences
    const double deltaAngle = std::fmod(currentValue - previousValue + 3.0 * M_PI, 2.0 * M_PI) - M_PI;

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

bool AngularCondition::IsNegativeCrossing(double currentValue, double previousValue, double targetValue)
{
    // Calculate angular differences
    const double deltaAngle = std::fmod(currentValue - previousValue + 3.0 * M_PI, 2.0 * M_PI) - M_PI;

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

std::function<bool(double, double, double)> AngularCondition::GenerateComparator(
    const AngularCondition::Criterion& aCriterion
)
{
    switch (aCriterion)
    {
        case Criterion::PositiveCrossing:
            return [](double currentValue, double previousValue, double aTarget) -> bool
            {
                return AngularCondition::IsPositiveCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::NegativeCrossing:
            return [](double currentValue, double previousValue, double aTarget) -> bool
            {
                return AngularCondition::IsNegativeCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::AnyCrossing:
            return [](double currentValue, double previousValue, double aTarget) -> bool
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
    const std::function<double(const State&)>& anEvaluator,
    const Pair<Angle, Angle>& aTargetRange
)
    : EventCondition(aName, anEvaluator, std::numeric_limits<double>::quiet_NaN()),
      criterion_(aCriterion),
      comparator_(
          [lowerBound = aTargetRange.first.inRadians(0.0, 2.0 * M_PI),
           upperBound = aTargetRange.second.inRadians(0.0, 2.0 * M_PI)](
              double currentValue, [[maybe_unused]] double previousValue, [[maybe_unused]] double aTarget
          ) -> bool
          {
              // For ranges that wrap around 0/360 (when lowerBound > upperBound)
              if (lowerBound > upperBound)
              {
                  return (currentValue >= lowerBound) || (currentValue <= upperBound);
              }

              // Normal range check
              return (currentValue >= lowerBound) && (currentValue <= upperBound);
          }
      ),
      targetRange_(
          std::make_pair(aTargetRange.first.inRadians(0.0, 2.0 * M_PI), aTargetRange.second.inRadians(0.0, 2.0 * M_PI))
      )
{
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
