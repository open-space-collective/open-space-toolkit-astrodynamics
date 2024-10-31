/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::time::Duration;

using ostk::astrodynamics::trajectory::State;

/// @brief An Event Condition that can evaluate each state to a real number. Meant for non-cyclic
/// continuous real numbers. For cyclic numbers (like angles), use AngularCondition.
class RealCondition : public EventCondition
{
   public:
    enum class Criterion
    {
        PositiveCrossing,
        NegativeCrossing,
        AnyCrossing,
        StrictlyPositive,
        StrictlyNegative
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  RealCondition RealCondition = {aName, aCriterion, anEvaluator, aTargetValue};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Real Event Condition
    /// @param aCriterion An enum indicating the criterion used to determine if the Real Event
    /// Condition is met
    /// @param anEvaluator A function evaluating a state
    /// @param aTargetValue A target value associated with the Real Event Condition
    RealCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)>& anEvaluator,
        const Real& aTargetValue = 0.0
    );

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  RealCondition RealCondition = {aName, aCriterion, anEvaluator, aTarget};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Real Event Condition
    /// @param aCriterion An enum indicating the criterion used to determine if the Real Event
    /// Condition is met
    /// @param anEvaluator A function evaluating a state
    /// @param aTarget A target associated with the Real Event Condition
    RealCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)>& anEvaluator,
        const EventCondition::Target& aTarget
    );

    /// @brief Virtual destructor
    virtual ~RealCondition();

    /// @brief Get the criterion of the Event Condition
    ///
    /// @return Enum representing the criterion of the Event Condition
    Criterion getCriterion() const;

    /// @brief Print the Event Condition
    ///
    /// @param [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    /// printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Evaluate the Event Condition
    ///
    /// @param aState The current state
    ///
    /// @return Real number representing the evaluation result of the Event Condition
    Real evaluate(const State& state) const;

    /// @brief Check if the Event Condition is satisfied based on current state and previous
    ///                  state/time
    ///
    /// @param currentState The current state
    /// @param previousState The previous state
    ///
    /// @return Boolean value indicating if the Event Condition is met
    virtual bool isSatisfied(const State& currentState, const State& previousState) const override;

    /// @brief clone the Real Condition
    ///
    /// @return Unique pointer to the cloned Real Condition
    virtual RealCondition* clone() const override;

    /// @brief Convert criterion to string
    ///
    /// @param aCriterion An enum representing the criterion
    ///
    /// @return String representing the given criterion
    static String StringFromCriterion(const Criterion& aCriterion);

    /// @brief Generate a Duration based condition
    ///
    /// @param aCriterion An enum representing the criterion
    /// @param aDuration A duration
    ///
    /// @return A Duration based condition
    static RealCondition DurationCondition(const Criterion& aCriterion, const Duration& aDuration);

   private:
    Criterion criterion_;
    std::function<bool(const Real&, const Real&)> comparator_;

    static std::function<bool(const Real&, const Real&)> GenerateComparator(const Criterion& aCriterion);
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
