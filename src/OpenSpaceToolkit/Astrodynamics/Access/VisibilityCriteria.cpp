/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriteria.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

VisibilityCriteria VisibilityCriteria::FromAERInterval(
    const Interval<Real>& azimuth, const Interval<Real>& elevation, const Interval<Real>& range
)
{
    return VisibilityCriteria(AERInterval {azimuth, elevation, range});
}

VisibilityCriteria VisibilityCriteria::FromAERMask(
    const Map<Real, Real>& azimuthElevationMask, const Interval<Real>& range
)
{
    return VisibilityCriteria(AERMask {azimuthElevationMask, range});
}

VisibilityCriteria VisibilityCriteria::FromLineOfSight(const Environment& environment)
{
    return VisibilityCriteria(LineOfSight {environment});
}

VisibilityCriteria VisibilityCriteria::FromElevationInterval(const Interval<Real>& anElevationInterval)
{
    return VisibilityCriteria(ElevationInterval {anElevationInterval});
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
