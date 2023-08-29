/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

using ostk::astro::eventcondition::RealEventCondition;

/// @brief                      A duration based event condition

class DurationCondition : public RealEventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         DurationCondition durationCondition = {aCriteria, aDuration};
    /// @endcode
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] startInstant An instant
    /// @param                  [in] aDuration A duration

    DurationCondition(const Criteria& aCriteria, const Instant& startInstant, const Duration& aDuration);

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
