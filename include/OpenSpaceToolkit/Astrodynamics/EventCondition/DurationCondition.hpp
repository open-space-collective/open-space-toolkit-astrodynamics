/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

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

using ostk::physics::time::Duration;

using ostk::astro::EventCondition;

/// @brief                      A duration based event condition

class DurationCondition : public EventCondition
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     DurationCondition durationCondition = {aCriteria, aDuration};
    /// @endcode
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aDuration A duration

    DurationCondition(const Criteria& aCriteria, const Duration& aDuration);

    /// @brief              Virtual destructor

    virtual ~DurationCondition();

    /// @brief              Get duration
    ///
    /// @return             Duration

    Duration getDuration() const;

    /// @brief              Evaluate the Event Condition
    ///
    /// @param              [in] aStateVector The current state vector
    /// @param              [in] aTime The current time
    ///
    /// @return             Real number representing the evaluation result of the Event Condition

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override;

   private:
    Real durationInSeconds_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif