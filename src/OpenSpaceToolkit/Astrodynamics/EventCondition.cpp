/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{

EventCondition::Target::Target(const Real& aValue, const Type& aType)
    : value_(aValue),
      type_(aType)
{
}

EventCondition::Target::Target(const Angle& anAngle, const Type& aType)
    : value_(anAngle.inRadians(0.0, Real::TwoPi())),
      type_(aType)
{
}

EventCondition::Target::Target(const Length& aLength, const Type& aType)
    : value_(aLength.inMeters()),
      type_(aType)
{
}

bool EventCondition::Target::operator==(const EventCondition::Target& aTarget) const
{
    if (!value_.isDefined() || !aTarget.value_.isDefined())
    {
        return false;
    }

    return (type_ == aTarget.type_) && (value_ == aTarget.value_);
}

bool EventCondition::Target::operator!=(const EventCondition::Target& aTarget) const
{
    return !((*this) == aTarget);
}

EventCondition::EventCondition(
    const String& aName, const std::function<Real(const State&)>& anEvaluator, const EventCondition::Target& aTarget
)
    : name_(aName),
      evaluator_(anEvaluator),
      target_(aTarget)
{
}

EventCondition::EventCondition(
    const String& aName, const std::function<Real(const State&)>& anEvaluator, const Real& aTargetValue
)
    : name_(aName),
      evaluator_(anEvaluator),
      target_({aTargetValue, EventCondition::Target::Type::Absolute})
{
}

EventCondition::~EventCondition() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition)
{
    anEventCondition.print(anOutputStream);

    return anOutputStream;
}

String EventCondition::getName() const
{
    return name_;
}

std::function<Real(const State&)> EventCondition::getEvaluator() const
{
    return evaluator_;
}

EventCondition::Target EventCondition::getTarget() const
{
    return target_;
}

void EventCondition::updateTarget(const State& aState)
{
    if (target_.type_ == EventCondition::Target::Type::Absolute)
    {
        throw ostk::core::error::RuntimeError("Can only set state for 'Relative' Event Conditions.");
    }

    if (!evaluator_)
    {
        throw ostk::core::error::runtime::Undefined("Evaluator");
    }

    target_.valueOffset_ = evaluator_(aState);
}

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << getName();
    // TBM: Use enum to string from Target::Type
    ostk::core::utils::Print::Line(anOutputStream
    ) << String::Format("Target {}:", ((target_.type_ == EventCondition::Target::Type::Absolute) ? " (Relative)" : ""))
      << getTarget().value_;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Target Type:" << (target_.type_ == EventCondition::Target::Type::Absolute ? "Relative" : "Absolute");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace astro
}  // namespace ostk
