/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

bool VisibilityCriterion::operator==(const VisibilityCriterion& aVisibilityCriterion) const
{
    return this->visibilityCriterion_ == aVisibilityCriterion.visibilityCriterion_;
}

bool VisibilityCriterion::operator!=(const VisibilityCriterion& aVisibilityCriterion) const
{
    return !(*this == aVisibilityCriterion);
}

VisibilityCriterion VisibilityCriterion::FromAERInterval(
    const Interval<double>& azimuth, const Interval<double>& elevation, const Interval<double>& range
)
{
    return VisibilityCriterion(AERInterval {azimuth, elevation, range});
}

VisibilityCriterion VisibilityCriterion::FromAERMask(
    const Map<double, double>& azimuthElevationMask, const Interval<double>& range
)
{
    return VisibilityCriterion(AERMask {azimuthElevationMask, range});
}

VisibilityCriterion VisibilityCriterion::FromLineOfSight(const Environment& environment)
{
    return VisibilityCriterion(LineOfSight {environment});
}

VisibilityCriterion VisibilityCriterion::FromElevationInterval(const Interval<double>& anElevationInterval)
{
    return VisibilityCriterion(ElevationInterval {anElevationInterval});
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
