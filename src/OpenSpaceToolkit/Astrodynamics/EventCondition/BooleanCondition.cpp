/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

BooleanCondition::BooleanCondition(
    const String& aName,
    const Criteria& aCriteria,
    const std::function<bool(const VectorXd&, const Real&)> anEvaluator,
    const bool& anInverseFlag
)
    : RealCondition(
          aName,
          aCriteria,
          [anInverseFlag, anEvaluator](const VectorXd& aStateVector, const Real& aTime) -> Real
          {
              const bool boolean = anEvaluator(aStateVector, aTime);
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
