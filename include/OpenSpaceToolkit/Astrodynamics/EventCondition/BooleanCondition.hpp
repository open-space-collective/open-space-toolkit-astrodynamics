/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition__

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::type::String;

using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::State;

/// @brief An Event Condition that can evaluate each state to a boolean.
class BooleanCondition : public RealCondition
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  BooleanCondition booleanCondition = {aName, aCriterion, anEvaluator,
    ///                  anInverseFlag};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Boolean Event Condition
    /// @param aCriterion An enum indicating the criterion used to determine if the Boolean Event
    /// Condition is met
    /// @param anEvaluator A function evaluating a state
    /// @param anInverseFlag A flag indicating whether the condition is inverted
    BooleanCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<bool(const State&)>& anEvaluator,
        const bool& anInverseFlag = false
    );

    /// @brief Virtual destructor
    virtual ~BooleanCondition();

    /// @brief Check if the condition is inversed
    ///
    /// @return Boolean value indicating whether the condition is inversed
    bool isInversed() const;

    /// @brief Print the Boolean Event Condition
    ///
    /// @param [in, out] anOutputStream The output stream where the Boolean Event Condition will be
    /// printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    /// printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Create a copy of this BooleanCondition
    ///
    /// @return Unique pointer to the cloned EventCondition
    virtual std::unique_ptr<EventCondition> clone() const override;

   private:
    bool inverse_;
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
