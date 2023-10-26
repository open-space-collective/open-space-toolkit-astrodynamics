/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{

EventCondition::EventCondition(
    const String& aName,
    std::function<Real(const State&)> anEvaluator,
    const Real& aTarget,
    const bool& targetIsRelative
)
    : name_(aName),
      evaluator_(anEvaluator),
      target_(aTarget),
      targetIsRelative_(targetIsRelative)
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

Real EventCondition::getTarget() const
{
    return target_;
}

bool EventCondition::targetIsRelative() const
{
    return targetIsRelative_;
}

void EventCondition::updateTarget(const State& aState)
{
    if (!targetIsRelative_)
    {
        throw ostk::core::error::RuntimeError("Can only set state for 'Relative' conditions.");
    }

    if (!evaluator_)
    {
        throw ostk::core::error::runtime::Undefined("Evaluator.");
    }

    relativeTarget_ = evaluator_(aState);
}

void EventCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Event Condition") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << getName();
    ostk::core::utils::Print::Line(anOutputStream)
        << String::Format("Target {}:", (targetIsRelative_ ? " (Relative)" : "")) << getTarget();
    ostk::core::utils::Print::Line(anOutputStream) << "Target Type:" << (targetIsRelative_ ? "Relative" : "Absolute");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace astro
}  // namespace ostk
