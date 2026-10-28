/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_CompositeGuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_CompositeGuidanceLaw__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

/// @brief    The Composite Guidance Law encompasses a sequence of one or more guidance laws to be used over
/// specific time intervals.
///    At each point in time, the applicable guidance law is selected and used to calculate the thrust acceleration.
///    Guidance laws don't need to be contiguous, and can be added in any order. If the instant does not fall within any
///    of the intervals, the thrust acceleration is zero. The guidance law intervals must not intersect each other.
class CompositeGuidanceLaw : public GuidanceLaw
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     CompositeGuidanceLaw guidanceLaw = {} ;
    ///     guidanceLaw.addGuidanceLaw(aConstantThrustSPtr, anInterval) ;
    /// @endcode
    ///
    /// @param aGuidanceLawWithIntervalArray Array of pairs containing the guidance law and their corresponding
    /// interval. Defaults to empty.
    CompositeGuidanceLaw(
        const Array<Pair<Shared<const GuidanceLaw>, Interval>>& aGuidanceLawWithIntervalArray =
            Array<Pair<Shared<const GuidanceLaw>, Interval>>::Empty()
    );

    /// @brief Get guidance laws with their corresponding intervals.
    ///
    /// @code{.cpp}
    ///     CompositeGuidanceLaw guidanceLaw = { ... } ;
    ///     auto lawsWithIntervals = guidanceLaw.getGuidanceLawsWithIntervals() ;
    /// @endcode
    ///
    /// @return Array of pairs containing the guidance laws and their corresponding intervals.
    Array<Pair<Shared<const GuidanceLaw>, Interval>> getGuidanceLawsWithIntervals() const;

    /// @brief Add a guidance law with its corresponding interval.
    ///
    /// @code{.cpp}
    ///     CompositeGuidanceLaw guidanceLaw = {} ;
    ///     guidanceLaw.addGuidanceLaw(aConstantThrustSPtr, anInterval) ;
    /// @endcode
    ///
    /// @param aGuidanceLawSPtr The guidance law to add.
    /// @param anInterval The interval during which the guidance law is active.
    void addGuidanceLaw(const Shared<const GuidanceLaw>& aGuidanceLawSPtr, const Interval& anInterval);

    /// @brief Calculate thrust acceleration at a given instant and state
    ///
    /// @param anInstant An instant
    /// @param aPositionCoordinates The position coordinates
    /// @param aVelocityCoordinates The velocity coordinates
    /// @param aThrustAcceleration The thrust acceleration
    /// @param outputFrameSPtr The frame in which the acceleration is expressed
    ///
    /// @return The acceleration at the provided coordinates
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief Construct an ungated composite guidance law from this instance.
    ///
    /// The new instance contains the same intervals paired with the ungated versions of the underlying guidance laws.
    ///
    /// @return The ungated guidance law instance.
    virtual Shared<GuidanceLaw> constructUngatedGuidanceLaw() const override;

   private:
    Array<Interval> intervals_;
    Array<Shared<const GuidanceLaw>> guidanceLaws_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
