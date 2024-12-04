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
    const Interval<Real>& azimuth, const Interval<Real>& elevation, const Interval<Real>& range
)
{
    return VisibilityCriterion(AERInterval {azimuth, elevation, range});
}

VisibilityCriterion VisibilityCriterion::FromAERMask(
    const Map<Real, Real>& azimuthElevationMask, const Interval<Real>& range
)
{
    return VisibilityCriterion(AERMask {azimuthElevationMask, range});
}

VisibilityCriterion VisibilityCriterion::FromLineOfSight(const Environment& environment)
{
    return VisibilityCriterion(LineOfSight {environment});
}

VisibilityCriterion VisibilityCriterion::FromElevationInterval(const Interval<Real>& anElevationInterval)
{
    return VisibilityCriterion(ElevationInterval {anElevationInterval});
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
