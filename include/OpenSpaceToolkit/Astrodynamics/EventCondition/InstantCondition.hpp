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

/// @brief A duration based event condition
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

    /// @brief Get instant
    ///
    /// @return Instant
    Instant getInstant() const;
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
