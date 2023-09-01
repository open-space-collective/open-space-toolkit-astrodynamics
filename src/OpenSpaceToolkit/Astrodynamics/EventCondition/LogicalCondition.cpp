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
    : EventCondition(aName),
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

bool LogicalCondition::isSatisfied(
    const VectorXd& currentStateVector,
    const Real& currentTime,
    const VectorXd& previousStateVector,
    const Real& previousTime
) const
{
    return evaluator_(eventConditions_, currentStateVector, currentTime, previousStateVector, previousTime);
}

LogicalCondition::evaluationSignature LogicalCondition::GenerateEvaluator(const LogicalCondition::Type& aType)
{
    switch (aType)
    {
        case LogicalCondition::Type::And:
            return [](const Array<Shared<EventCondition>>& eventConditions,
                      const VectorXd& currentStateVector,
                      const Real& currentTime,
                      const VectorXd& previousStateVector,
                      const Real& previousTime) -> bool
            {
                return std::all_of(
                    eventConditions.begin(),
                    eventConditions.end(),
                    [&currentStateVector, &currentTime, &previousStateVector, &previousTime](
                        const Shared<EventCondition>& eventCondition
                    ) -> bool
                    {
                        return eventCondition->isSatisfied(
                            currentStateVector, currentTime, previousStateVector, previousTime
                        );
                    }
                );
            };

        case LogicalCondition::Type::Or:
            return [](const Array<Shared<EventCondition>>& eventConditions,
                      const VectorXd& currentStateVector,
                      const Real& currentTime,
                      const VectorXd& previousStateVector,
                      const Real& previousTime) -> bool
            {
                return std::any_of(
                    eventConditions.begin(),
                    eventConditions.end(),
                    [&currentStateVector, &currentTime, &previousStateVector, &previousTime](
                        const Shared<EventCondition>& eventCondition
                    ) -> bool
                    {
                        return eventCondition->isSatisfied(
                            currentStateVector, currentTime, previousStateVector, previousTime
                        );
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
