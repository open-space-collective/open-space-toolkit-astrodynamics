/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

/// @brief                      An Event Condition defines a criteria that can be evaluated
///                             based on a current/previous state vectors and times

class EventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         EventCondition eventCondition = {aName, aCriteria, anEvaluatro, aTarget};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Real Event
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
    /// printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Check if the Event Condition is satisfied based on current state/time and previous
    /// state/time
    ///
    /// @param                  [in] currentStateVector The current state vector
    /// @param                  [in] currentTime The current time
    /// @param                  [in] previousStateVector The previous state vector
    /// @param                  [in] previousTime The previous time
    ///
    /// @return                 Boolean value indicating if the Event Condition is met

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const = 0;

   private:
    String name_;
};

}  // namespace astro
}  // namespace ostk

#endif
