/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astrodynamics
{

EventCondition::Target::Target(double aValue, const Type& aType)
    : value(aValue),
      type(aType)
{
}

EventCondition::Target::Target(const Angle& anAngle, const Type& aType)
    : value(anAngle.inRadians(0.0, 2.0 * M_PI)),
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
    return (type == aTarget.type) && (value == aTarget.value);
}

bool EventCondition::Target::operator!=(const EventCondition::Target& aTarget) const
{
    return !((*this) == aTarget);
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
    const String& aName, const std::function<double(const State&)>& anEvaluator, const EventCondition::Target& aTarget
)
    : name_(aName),
      evaluator_(anEvaluator),
      target_(aTarget)
{
}

EventCondition::EventCondition(
    const String& aName, const std::function<double(const State&)>& anEvaluator, double aTargetValue
)
    : name_(aName),
      evaluator_(anEvaluator),
      target_(EventCondition::Target(aTargetValue, EventCondition::Target::Type::Absolute))
{
}

EventCondition::~EventCondition() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition)
{
    anEventCondition.print(anOutputStream);

    return anOutputStream;
}

const String& EventCondition::getName() const
{
    return name_;
}

const std::function<double(const State&)>& EventCondition::getEvaluator() const
{
    return evaluator_;
}

const EventCondition::Target& EventCondition::getTarget() const
{
    if (!target_.has_value())
    {
        throw ostk::core::error::runtime::Undefined("Target");
    }

    return target_.value();
}

void EventCondition::updateTarget(const State& aState)
{
    if (!evaluator_)
    {
        throw ostk::core::error::runtime::Undefined("Evaluator");
    }

    if (target_.has_value() && (target_->type == EventCondition::Target::Type::Relative))
    {
        target_->valueOffset = evaluator_(aState);
    }
}

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    if (target_.has_value())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Target:" << target_->value;
        ostk::core::utils::Print::Line(anOutputStream)
            << "Target Type:" << EventCondition::Target::StringFromType(target_->type);
    }
    else
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Target:" << "Undefined";
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

EventCondition::EventCondition(const String& aName, const std::function<double(const State&)>& anEvaluator)
    : name_(aName),
      evaluator_(anEvaluator),
      target_(std::nullopt)
{
}

}  // namespace astrodynamics
}  // namespace ostk
