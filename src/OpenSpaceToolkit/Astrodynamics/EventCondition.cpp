/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astrodynamics
{

EventCondition::Target::Target(const Real& aValue, const Type& aType)
    : value(aValue),
      type(aType)
{
}

EventCondition::Target::Target(const Angle& anAngle, const Type& aType)
    : value(anAngle.inRadians(0.0, Real::TwoPi())),
      type(aType)
{
}

EventCondition::Target::Target(const Length& aLength, const Type& aType)
    : value(aLength.inMeters()),
      type(aType)
{
}

bool EventCondition::Target::operator==(const EventCondition::Target& aTarget) const
{
    if (!value.isDefined() || !aTarget.value.isDefined())
    {
        return false;
    }

    return (type == aTarget.type) && (value == aTarget.value);
}

bool EventCondition::Target::operator!=(const EventCondition::Target& aTarget) const
{
    return !((*this) == aTarget);
}

bool EventCondition::Target::isDefined() const
{
    return value.isDefined();
}

String EventCondition::Target::StringFromType(const EventCondition::Target::Type& aType)
{
    switch (aType)
    {
        case EventCondition::Target::Type::Absolute:
            return "Absolute";
        case EventCondition::Target::Type::Relative:
            return "Relative";
        default:
            throw ostk::core::error::runtime::Wrong("Type");
    }
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
    if (!evaluator_)
    {
        throw ostk::core::error::runtime::Undefined("Evaluator");
    }

    if (target_.type == EventCondition::Target::Type::Relative)
    {
        target_.valueOffset = evaluator_(aState);
    }
}

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    ostk::core::utils::Print::Line(anOutputStream) << "Target:" << target_.value;
    ostk::core::utils::Print::Line(anOutputStream)
        << "Target Type:" << EventCondition::Target::StringFromType(target_.type);
    if (target_.type == EventCondition::Target::Type::Relative)
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Target Value Offset:" << target_.valueOffset;
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace astrodynamics
}  // namespace ostk
