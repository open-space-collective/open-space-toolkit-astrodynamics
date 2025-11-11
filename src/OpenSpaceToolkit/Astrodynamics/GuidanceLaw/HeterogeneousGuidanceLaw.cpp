/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>

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
using ostk::core::type::Size;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;

HeterogeneousGuidanceLaw::HeterogeneousGuidanceLaw(
    const Array<Pair<Shared<const GuidanceLaw>, Interval>>& aGuidanceLawWithIntervalArray
)
    : GuidanceLaw("Heterogeneous Guidance Law"),
      intervals_(Array<Interval>::Empty()),
      guidanceLaws_(Array<Shared<const GuidanceLaw>>::Empty())
{
    for (const Pair<Shared<const GuidanceLaw>, Interval>& guidanceLawWithInterval : aGuidanceLawWithIntervalArray)
    {
        this->addGuidanceLaw(guidanceLawWithInterval.first, guidanceLawWithInterval.second);
    }
}

Array<Pair<Shared<const GuidanceLaw>, Interval>> HeterogeneousGuidanceLaw::getGuidanceLawsWithIntervals() const
{
    Array<Pair<Shared<const GuidanceLaw>, Interval>> guidanceLawsWithIntervals =
        Array<Pair<Shared<const GuidanceLaw>, Interval>>::Empty();
    for (Size i = 0; i < intervals_.getSize(); ++i)
    {
        guidanceLawsWithIntervals.add(Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaws_[i], intervals_[i]));
    }
    return guidanceLawsWithIntervals;
}

void HeterogeneousGuidanceLaw::addGuidanceLaw(
    const Shared<const GuidanceLaw>& aGuidanceLawSPtr, const Interval& anInterval
)
{
    if (aGuidanceLawSPtr == nullptr)
    {
        throw ostk::core::error::RuntimeError("Guidance law cannot be null.");
    }

    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    for (const Interval& interval : intervals_)
    {
        if (interval.intersects(anInterval))
        {
            throw ostk::core::error::RuntimeError(ostk::core::type::String::Format(
                "Interval intersects: {} and {}", interval.toString(), anInterval.toString()
            ));
        }
    }

    intervals_.add(anInterval);
    guidanceLaws_.add(aGuidanceLawSPtr);
}

Vector3d HeterogeneousGuidanceLaw::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    // Check each interval to find which guidance law to use
    for (Size i = 0; i < intervals_.getSize(); ++i)
    {
        if (intervals_[i].contains(anInstant))
        {
            return guidanceLaws_[i]->calculateThrustAccelerationAt(
                anInstant, aPositionCoordinates, aVelocityCoordinates, aThrustAcceleration, outputFrameSPtr
            );
        }
    }

    return Vector3d {0.0, 0.0, 0.0};
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
