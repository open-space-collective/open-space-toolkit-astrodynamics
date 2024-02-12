/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access__
#define __OpenSpaceToolkit_Astrodynamics_Access__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::container::Array;
using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::physics::unit::Angle;

/// @brief Object-to-object visibility
class Access
{
   public:
    enum class Type
    {

        Undefined,
        Complete,
        Partial

    };

    Access(
        const Access::Type& aType,
        const Instant& anAcquisitionOfSignal,
        const Instant& aTimeOfClosestApproach,
        const Instant& aLossOfSignal,
        const Angle& aMaxElevation
    );

    bool operator==(const Access& anAccess) const;

    bool operator!=(const Access& anAccess) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Access& anAccess);

    bool isDefined() const;

    bool isComplete() const;

    Access::Type getType() const;

    Instant getAcquisitionOfSignal() const;

    Instant getTimeOfClosestApproach() const;

    Instant getLossOfSignal() const;

    Interval getInterval() const;

    Duration getDuration() const;

    Angle getMaxElevation() const;

    static Access Undefined();

    static String StringFromType(const Access::Type& aType);

   private:
    Access::Type type_;

    Instant acquisitionOfSignal_;
    Instant timeOfClosestApproach_;
    Instant lossOfSignal_;
    Angle maxElevation_;
};

}  // namespace astro
}  // namespace ostk

#endif
