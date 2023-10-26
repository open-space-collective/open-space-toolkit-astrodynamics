/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::types::String;
using ostk::core::types::Real;

using ostk::astro::trajectory::State;

/// @brief                      An Event Condition defines a criterion that can be evaluated
///                             based on a current/previous state vectors and times

class EventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         EventCondition eventCondition = {aName, anEvaluator, aTarget, targetIsRelative};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] anEvaluator A function evaluating a state
    /// @param                  [in] aTarget A target value associated with the Real Event Condition
    /// @param                  [in] targetIsRelative A boolean indicating whether or not the target is relative

    EventCondition(
        const String& aName,
        std::function<Real(const State&)> anEvaluator,
        const Real& aTarget,
        const bool& targetIsRelative = false
    );

    /// @brief                  Virtual destructor

    virtual ~EventCondition();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << EventCondition(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] anEventCondition An EventCondition
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition);

    /// @brief                  Get the name of the Event Condition
    ///
    /// @return                 String representing the name of the Event Condition

    String getName() const;

    /// @brief                  Get evaluator
    ///
    /// @return                 Evaluator

    std::function<Real(const State&)> getEvaluator() const;

    /// @brief                  Get the target
    ///
    /// @return                 Target

    Real getTarget() const;

    /// @brief                  Get the target is relative
    ///
    /// @return                 Target is relative

    bool targetIsRelative() const;

    /// @brief                  Update the target value if the Event Condition is relative
    ///
    /// @param                  [in] aState A state to calculate the relative target from

    virtual void updateTarget(const State& aState);

    /// @brief                  Print the Event Condition
    ///
    /// @param                  [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display decorator during
    ///                         printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Check if the Event Condition is satisfied based on current state and previous
    ///                         state/time
    ///
    /// @param                  [in] currentState The current state
    /// @param                  [in] previousState The previous state
    ///
    /// @return                 Boolean value indicating if the Event Condition is met

    virtual bool isSatisfied(const State& currentState, const State& previousState) const = 0;

   protected:
    String name_;
    std::function<Real(const State&)> evaluator_;
    Real target_;
    mutable Real relativeTarget_ = 0.0;
    bool targetIsRelative_;
};

}  // namespace astro
}  // namespace ostk

#endif
