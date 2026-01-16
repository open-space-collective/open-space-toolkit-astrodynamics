/// Apache License 2.0

#include <cmath>

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
    const std::function<Real(const State&)>& anEvaluator,
    const Angle& aTargetAngle
)
    : EventCondition(aName, anEvaluator, aTargetAngle.inRadians(0.0, Real::TwoPi())),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion)),
      targetRange_(std::make_pair(Real::Undefined(), Real::Undefined()))
{
}

AngularCondition::AngularCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<Real(const State&)>& anEvaluator,
    const Target& aTarget
)
    : EventCondition(aName, anEvaluator, aTarget),
      criterion_(aCriterion),
      comparator_(GenerateComparator(aCriterion)),
      targetRange_(std::make_pair(Real::Undefined(), Real::Undefined()))
{
}

AngularCondition::~AngularCondition() {}

AngularCondition::Criterion AngularCondition::getCriterion() const
{
    return criterion_;
}

Angle AngularCondition::getTargetAngle() const
{
    if (!target_.value.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Target");
    }

    return Angle::Radians(target_.value);
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

Real AngularCondition::evaluate(const State& aState) const
{
    return NormalizedAngularDistance(evaluator_(aState), target_.value + target_.valueOffset);
}

bool AngularCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return comparator_(evaluator_(currentState), evaluator_(previousState), (target_.value + target_.valueOffset));
}

AngularCondition* AngularCondition::clone() const
{
    return new AngularCondition(*this);
}

bool AngularCondition::evaluateNegativeWhenSatisfied() const
{
    return criterion_ == Criterion::NegativeCrossing;
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

Real AngularCondition::NormalizedAngularDistance(const Real& angle, const Real& target)
{
    // Compute the signed angular difference, normalized to [-π, π)
    // Positive result means angle is "ahead" of target (in positive angular direction)
    // Negative result means angle is "behind" target
    return std::fmod(angle - target + 3.0 * Real::Pi(), Real::TwoPi()) - Real::Pi();
}

bool AngularCondition::IsPositiveCrossing(const Real& currentValue, const Real& previousValue, const Real& targetValue)
{
    const Real previousAngularDistance = NormalizedAngularDistance(previousValue, targetValue);
    const Real currentAngularDistance = NormalizedAngularDistance(currentValue, targetValue);

    // Compute direction of movement
    const Real deltaAngle = NormalizedAngularDistance(currentValue, previousValue);

    // Positive crossing: moving in positive direction and crossed from behind target to at/ahead of target
    return (deltaAngle > 0.0) && (previousAngularDistance < 0.0) && (currentAngularDistance >= 0.0);
}

bool AngularCondition::IsNegativeCrossing(const Real& currentValue, const Real& previousValue, const Real& targetValue)
{
    const Real previousAngularDistance = NormalizedAngularDistance(previousValue, targetValue);
    const Real currentAngularDistance = NormalizedAngularDistance(currentValue, targetValue);

    // Compute direction of movement
    const Real deltaAngle = NormalizedAngularDistance(currentValue, previousValue);

    // Negative crossing: moving in negative direction and crossed from ahead of target to at/behind target
    return (deltaAngle < 0.0) && (previousAngularDistance > 0.0) && (currentAngularDistance <= 0.0);
}

std::function<bool(const Real&, const Real&, const Real&)> AngularCondition::GenerateComparator(
    const AngularCondition::Criterion& aCriterion
)
{
    switch (aCriterion)
    {
        case Criterion::PositiveCrossing:
            return [](const Real& currentValue, const Real& previousValue, const Real& aTarget) -> bool
            {
                return AngularCondition::IsPositiveCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::NegativeCrossing:
            return [](const Real& currentValue, const Real& previousValue, const Real& aTarget) -> bool
            {
                return AngularCondition::IsNegativeCrossing(currentValue, previousValue, aTarget);
            };
        case Criterion::AnyCrossing:
            return [](const Real& currentValue, const Real& previousValue, const Real& aTarget) -> bool
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
    : EventCondition(aName, anEvaluator, Real::Undefined()),
      criterion_(aCriterion),
      comparator_(
          [lowerBound = aTargetRange.first.inRadians(0.0, Real::TwoPi()),
           upperBound = aTargetRange.second.inRadians(0.0, Real::TwoPi())](
              const Real& currentValue, [[maybe_unused]] const Real& previousValue, [[maybe_unused]] const Real& aTarget
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
      targetRange_(std::make_pair(
          aTargetRange.first.inRadians(0.0, Real::TwoPi()), aTargetRange.second.inRadians(0.0, Real::TwoPi())
      ))
{
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
