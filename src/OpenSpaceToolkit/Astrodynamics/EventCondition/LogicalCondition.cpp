/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

LogicalCondition::LogicalCondition(
    const String& aName, const LogicalCondition::Type& aType, const Array<Shared<EventCondition>>& eventConditions
)
    : EventCondition(aName, nullptr, 0.0, false),
      type_(aType),
      eventConditions_(eventConditions),
      evaluator_(LogicalCondition::GenerateEvaluator(aType))
{
}

LogicalCondition::~LogicalCondition() {}

LogicalCondition::Type LogicalCondition::getType() const
{
    return type_;
}

Array<Shared<EventCondition>> LogicalCondition::getEventConditions() const
{
    return eventConditions_;
}

void LogicalCondition::updateTarget(const State& aState)
{
    for (const auto& eventCondition : eventConditions_)
    {
        eventCondition->updateTarget(aState);
    }
}

bool LogicalCondition::isSatisfied(const State& currentState, const State& previousState) const
{
    return evaluator_(eventConditions_, currentState, previousState);
}

LogicalCondition::evaluationSignature LogicalCondition::GenerateEvaluator(const LogicalCondition::Type& aType)
{
    switch (aType)
    {
        case LogicalCondition::Type::And:
            return [](const Array<Shared<EventCondition>>& eventConditions,
                      const State& currentState,
                      const State& previousState) -> bool
            {
                return std::all_of(
                    eventConditions.begin(),
                    eventConditions.end(),
                    [&currentState, &previousState](const Shared<EventCondition>& eventCondition) -> bool
                    {
                        return eventCondition->isSatisfied(currentState, previousState);
                    }
                );
            };

        case LogicalCondition::Type::Or:
            return [](const Array<Shared<EventCondition>>& eventConditions,
                      const State& currentState,
                      const State& previousState) -> bool
            {
                return std::any_of(
                    eventConditions.begin(),
                    eventConditions.end(),
                    [&currentState, &previousState](const Shared<EventCondition>& eventCondition) -> bool
                    {
                        return eventCondition->isSatisfied(currentState, previousState);
                    }
                );
            };

        default:
            throw ostk::core::error::runtime::Wrong("Type");
    }
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
