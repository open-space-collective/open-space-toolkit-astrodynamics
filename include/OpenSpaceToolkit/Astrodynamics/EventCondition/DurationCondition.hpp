/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

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

using ostk::astro::eventcondition::RealCondition;

/// @brief                      A duration based event condition

class DurationCondition : public RealCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         DurationCondition durationCondition = {aCriterion, aDuration};
    /// @endcode
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aDuration A duration

    DurationCondition(const Criterion& aCriterion, const Duration& aDuration);

    /// @brief                  Virtual destructor

    virtual ~DurationCondition();

    /// @brief                  Get duration
    ///
    /// @return                 Duration

    Duration getDuration() const;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
