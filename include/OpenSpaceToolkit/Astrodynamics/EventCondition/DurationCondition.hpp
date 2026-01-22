/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_DurationCondition__

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::type::Shared;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::RealCondition;

/// @brief A duration based event condition
class DurationCondition : public RealCondition
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  DurationCondition durationCondition = {aCriterion, aDuration, aReferenceInstant};
    /// @endcode
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aDuration A duration
    /// @param aReferenceInstant An instant to use as the reference instant. Defaults to J2000. Should ideally be set to
    /// the start instant from which the duration is measured.
    DurationCondition(
        const Criterion& aCriterion, const Duration& aDuration, const Instant& aReferenceInstant = Instant::J2000()
    );

    /// @brief Virtual destructor
    virtual ~DurationCondition();

    /// @brief Get the duration
    ///
    /// @return Duration
    Duration getDuration() const;

    /// @brief Get the reference instant, from which the duration is measured.
    ///
    /// @return Reference instant
    Instant getReferenceInstant() const;

    /// @brief Update the reference instant
    ///
    /// @param aState The state to use to update the reference instant
    virtual void updateTarget(const State& aState) override;

   private:
    /// @brief Private delegating constructor that takes a shared_ptr to the reference instant
    DurationCondition(
        const Criterion& aCriterion, const Duration& aDuration, const Shared<Instant>& aReferenceInstantSPtr
    );

    Shared<Instant> referenceInstantSPtr_;
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
