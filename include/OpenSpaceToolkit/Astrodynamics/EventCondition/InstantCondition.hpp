/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_InstantCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_InstantCondition__

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::RealCondition;

/// @brief An instant-based event condition.
///
/// @details Triggers when the propagation reaches a specified instant, using a RealCondition
/// that compares elapsed seconds since the reference instant.
class InstantCondition : public RealCondition
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  InstantCondition instantCondition = {aCriterion, anInstant};
    /// @endcode
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aDuration An instant
    InstantCondition(const Criterion& aCriterion, const Instant& anInstant);

    /// @brief Virtual destructor
    virtual ~InstantCondition();

    /// @brief Get the target instant.
    ///
    /// @code{.cpp}
    ///     InstantCondition condition = { ... } ;
    ///     Instant instant = condition.getInstant() ;
    /// @endcode
    ///
    /// @return The target instant.
    Instant getInstant() const;

   private:
    Instant instant_;
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
