/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw__

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

/// @brief    The Heterogeneous Guidance Law encompasses a sequence of one or more guidance laws to be used over
/// specific time intervals.
///    At each point in time, the applicable guidance law is selected and used to calculate the thrust acceleration.
///    Guidance laws don't need to be contiguous, and can be added in any order. If the instant does not fall within any
///    of the intervals, the thrust acceleration is zero. The guidance law intervals must not intersect each other.
class HeterogeneousGuidanceLaw : public GuidanceLaw
{
   public:
    /// @brief Default constructor
    ///
    /// @param aGuidanceLawWithIntervalArray Array of tuples containing the guidance law and their corresponding
    /// interval. Defaults to empty.
    HeterogeneousGuidanceLaw(
        const Array<Pair<Shared<GuidanceLaw>, Interval>>& aGuidanceLawWithIntervalArray =
            Array<Pair<Shared<GuidanceLaw>, Interval>>::Empty()
    );

    /// @brief Get guidance laws with their corresponding intervals
    ///
    /// @return Array of tuples containing the guidance laws and their corresponding intervals
    Array<Pair<Shared<GuidanceLaw>, Interval>> getGuidanceLawsWithIntervals() const;

    /// @brief Add a guidance law with its corresponding interval
    ///
    /// @param aGuidanceLawSPtr The guidance law to add
    /// @param anInterval The interval during which the guidance law is active
    void addGuidanceLaw(const Shared<GuidanceLaw>& aGuidanceLawSPtr, const Interval& anInterval);

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

   private:
    Array<Interval> intervals_;
    Array<Shared<GuidanceLaw>> guidanceLaws_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
