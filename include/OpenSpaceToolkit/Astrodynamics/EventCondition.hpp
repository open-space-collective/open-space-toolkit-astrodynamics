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

using ostk::astro::trajectory::State;

/// @brief                      An Event Condition defines a criterion that can be evaluated
///                             based on a current/previous state vectors and times

class EventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         EventCondition eventCondition = {aName, aCriterion, anEvaluatro, aTarget};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine if the Real Event
    /// Condition is met
    /// @param                  [in] anEvaluator A function evaluating a state and a time
    /// @param                  [in] aTarget A target value associated with the Real Event Condition

    EventCondition(const String& aName);

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

   private:
    String name_;
};

}  // namespace astro
}  // namespace ostk

#endif
