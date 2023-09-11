/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::astro::trajectory::State;

/// @brief                      An Event Condition that can evaluate each state to a real number. Meant for non-cyclic
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

    /// @brief                  Constructor
    ///
    /// @code
    ///                         RealCondition RealCondition = {aName, aCriterion, anEvaluatro, aTarget};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine if the Real Event
    /// Condition is met
    /// @param                  [in] anEvaluator A function evaluating a state
    /// @param                  [in] aTarget A target value associated with the Real Event Condition

    RealCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)> anEvaluator,
        const Real& aTarget = 0.0
    );

    /// @brief                  Virtual destructor

    virtual ~RealCondition();

    /// @brief                  Get the criterion of the Event Condition
    ///
    /// @return                 Enum representing the criterion of the Event Condition

    Criterion getCriterion() const;

    /// @brief                  Get evaluator
    ///
    /// @return                 Evaluator

    std::function<Real(const State&)> getEvaluator() const;

    /// @brief                  Get the target of the Event Condition
    ///
    /// @return                 Real number representing the target of the Event Condition

    Real getTarget() const;

    /// @brief                  Print the Event Condition
    ///
    /// @param                  [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display decorator during
    /// printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Evaluate the Event Condition
    ///
    /// @param                  [in] aState The current state
    ///
    /// @return                 Real number representing the evaluation result of the Event Condition

    Real evaluate(const State& state) const;

    /// @brief                  Check if the Event Condition is satisfied based on current state and previous
    ///                         state/time
    ///
    /// @param                  [in] currentState The current state
    /// @param                  [in] previousState The previous state
    ///
    /// @return                 Boolean value indicating if the Event Condition is met

    virtual bool isSatisfied(const State& currentState, const State& previousState) const override;

    /// @brief                  Convert criterion to string
    ///
    /// @param                  [in] aCriterion An enum representing the criterion
    ///
    /// @return                 String representing the given criterion

    static String StringFromCriterion(const Criterion& aCriterion);

   private:
    Criterion criterion_;
    std::function<Real(const State&)> evaluator_;
    Real target_;
    std::function<bool(const Real&, const Real&)> comparator_;

    static std::function<bool(const Real&, const Real&)> GenerateComparator(const Criterion& aCriterion);
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
