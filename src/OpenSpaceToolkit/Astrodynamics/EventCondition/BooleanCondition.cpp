/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

BooleanCondition::BooleanCondition(
    const String& aName,
    const Criterion& aCriterion,
    const std::function<bool(const State&)>& anEvaluator,
    const bool& anInverseFlag
)
    : RealCondition(
          aName,
          aCriterion,
          [anInverseFlag, anEvaluator](const State& aState) -> Real
          {
              const bool boolean = anEvaluator(aState);
              return anInverseFlag ? (boolean ? -1.0 : 1.0) : (boolean ? 1.0 : -1.0);
          },
          0.0
      ),
      inverse_(anInverseFlag)
{
}

BooleanCondition::~BooleanCondition() {}

bool BooleanCondition::isInversed() const
{
    return inverse_;
}

void BooleanCondition::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Boolean Condition") : void();

    RealCondition::print(anOutputStream, false);
    ostk::core::utils::Print::Line(anOutputStream) << "isInversed:" << (isInversed() ? "True" : "False");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
