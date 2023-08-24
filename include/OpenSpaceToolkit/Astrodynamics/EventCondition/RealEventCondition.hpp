/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_RealEventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_RealEventCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;

/// @brief                      A Real Event Condition is a condition that is met when computed value matches the
/// criteria at a target value

class RealEventCondition : public EventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         RealEventCondition realEventCondition = {aName, aCriteria};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Real Event
    /// Condition is met
    /// @param                  [in] aTarget A target value associated with the Real Event Condition

    RealEventCondition(const String& aName, const Criteria& aCriteria, const Real& aTarget);

    /// @brief                  Virtual destructor

    virtual ~RealEventCondition();

    /// @brief                  Get the target of the Event Condition
    ///
    /// @return                 Real number representing the target of the Event Condition

    Real getTarget() const;

    /// @brief                  Check if the Real Event Condition is satisfied based on current state/time and previous
    /// state/time
    ///
    /// @param                  [in] currentStateVector The current state vector
    /// @param                  [in] currentTime The current time
    /// @param                  [in] previousStateVector The previous state vector
    /// @param                  [in] previousTime The previous time
    ///
    /// @return                 Boolean value indicating if the Real Event Condition is met

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override;

    /// @brief                  Evaluate the Event Condition
    ///
    /// @param                  [in] aStateVector The current state vector
    /// @param                  [in] aTime The current time
    ///
    /// @return                 Real number representing the evaluation result of the Event Condition

    Real evaluate(const VectorXd& aStateVector, const Real& aTime) const;

    /// @brief                  Compute the value of the Real Event Condition
    ///
    /// @param                  [in] aStateVector The state vector
    /// @param                  [in] aTime The time
    ///
    /// @return                 Real number representing the value of the Event Condition

    virtual Real compute(const VectorXd& aStateVector, const Real& aTime) const = 0;

   private:
    Real target_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
