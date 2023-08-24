/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanEventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanEventCondition__

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

/// @brief                      A Boolean Event Condition is a condition that is met when the computed value returns
/// true

class BooleanEventCondition : public EventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         BooleanEventCondition booleanEventCondition = {aName, aCriteria};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Boolean Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Boolean Event
    /// Condition is met
    /// @param                  [in] anInverseFlag A flag indicating whether the condition is inverted

    BooleanEventCondition(const String& aName, const Criteria& aCriteria, const bool& anInverseFlag = false);

    /// @brief                  Virtual destructor

    virtual ~BooleanEventCondition();

    /// @brief                  Check if the condition is inversed
    ///
    /// @return                 Boolean value indicating whether the condition is inversed

    Real isInversed() const;

    /// @brief                  Check if the Event Condition is satisfied based on current state/time and previous
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

    /// @brief                  Compute the value of the Boolean Event Condition
    ///
    /// @param                  [in] aStateVector The state vector
    /// @param                  [in] aTime The time
    ///
    /// @return                 Boolean value representing the value of the Event Condition

    virtual bool compute(const VectorXd& aStateVector, const Real& aTime) const = 0;

   private:
    bool inverse_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
