/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::type::String;

using ostk::mathematics::object::Vector2d;

VisibilityCriterion::AERMask::AERMask(
    const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeInterval
)
    : azimuthElevationMask(anAzimuthElevationMask),
      range(aRangeInterval)
{
    if (range.getLowerBound() < 0.0)
    {
        throw ostk::core::error::RuntimeError(
            "The range interval [{}, {}] must be positive.", range.accessLowerBound(), range.accessUpperBound()
        );
    }

    if ((anAzimuthElevationMask.empty()) || (anAzimuthElevationMask.begin()->first < 0.0) ||
        (anAzimuthElevationMask.rbegin()->first > 360.0))
    {
        throw ostk::core::error::runtime::Wrong(
            "Azimuth-Elevation Mask",
            String::Format(
                "Empty: {}, Min azimuth: {}, Max azimuth: {}",
                anAzimuthElevationMask.empty(),
                anAzimuthElevationMask.empty() ? Real::Zero() : anAzimuthElevationMask.begin()->first,
                anAzimuthElevationMask.empty() ? Real::Zero() : anAzimuthElevationMask.rbegin()->first
            )
        );
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if ((azimuthElevationPair.second).abs() > 90.0)
        {
            throw ostk::core::error::runtime::Wrong(
                "Azimuth-Elevation Mask",
                String::Format("Azimuth: {}, Elevation: {}", azimuthElevationPair.first, azimuthElevationPair.second)
            );
        }
    }

    Map<Real, Real> anAzimuthElevationMask_deg = anAzimuthElevationMask;

    if (anAzimuthElevationMask_deg.begin()->first != 0.0)
    {
        anAzimuthElevationMask_deg.insert({0.0, anAzimuthElevationMask_deg.begin()->second});
    }

    if (anAzimuthElevationMask_deg.rbegin()->first != 360.0)
    {
        anAzimuthElevationMask_deg.insert({360.0, anAzimuthElevationMask_deg.begin()->second});
    }

    Map<Real, Real> anAzimuthElevationMask_rad;

    for (auto it = anAzimuthElevationMask_deg.begin(); it != anAzimuthElevationMask_deg.end(); ++it)
    {
        anAzimuthElevationMask_rad.insert({it->first * M_PI / 180.0, it->second * M_PI / 180.0});
    }

    this->azimuthElevationMask = anAzimuthElevationMask_rad;
}

bool VisibilityCriterion::AERMask::isSatisfied(const AER& anAer) const
{
    return isSatisfied(
        anAer.getAzimuth().inRadians(0.0, Real::TwoPi()),
        anAer.getElevation().inRadians(-Real::Pi(), Real::Pi()),
        anAer.getRange().inMeters()
    );
}

bool VisibilityCriterion::AERMask::isSatisfied(
    const Real& anAzimuth_Radians, const Real& anElevation_Radians, const Real& aRange_Meters
) const
{
    auto itLow = this->azimuthElevationMask.lower_bound(anAzimuth_Radians);
    itLow--;
    auto itUp = this->azimuthElevationMask.upper_bound(anAzimuth_Radians);

    // Vector between the two successive mask data points with bounding azimuth values

    const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

    // Vector from data point with azimuth lower bound to tested point

    const Vector2d lowToPointVector = {anAzimuth_Radians - itLow->first, anElevation_Radians - itLow->second};

    // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
    // mask

    return ((lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0]) >= 0.0) &&
           range.contains(aRange_Meters);
}

bool VisibilityCriterion::AERMask::operator==(const AERMask& anAerMask) const
{
    return this->azimuthElevationMask == anAerMask.azimuthElevationMask && this->range == anAerMask.range;
}

bool VisibilityCriterion::AERMask::operator!=(const AERMask& anAerMask) const
{
    return !((*this) == anAerMask);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
