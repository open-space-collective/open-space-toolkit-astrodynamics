/// Apache License 2.0

#include <cmath>

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
    const Map<double, double>& anAzimuthElevationMask, const Interval<Real>& aRangeInterval
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
                anAzimuthElevationMask.empty() ? 0.0 : anAzimuthElevationMask.begin()->first,
                anAzimuthElevationMask.empty() ? 0.0 : anAzimuthElevationMask.rbegin()->first
            )
        );
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if (std::abs((azimuthElevationPair.second)) > 90.0)
        {
            throw ostk::core::error::runtime::Wrong(
                "Azimuth-Elevation Mask",
                String::Format("Azimuth: {}, Elevation: {}", azimuthElevationPair.first, azimuthElevationPair.second)
            );
        }
    }

    Map<double, double> anAzimuthElevationMask_deg = anAzimuthElevationMask;

    if (anAzimuthElevationMask_deg.begin()->first != 0.0)
    {
        anAzimuthElevationMask_deg.insert({0.0, anAzimuthElevationMask_deg.begin()->second});
    }

    if (anAzimuthElevationMask_deg.rbegin()->first != 360.0)
    {
        anAzimuthElevationMask_deg.insert({360.0, anAzimuthElevationMask_deg.begin()->second});
    }

    Map<double, double> anAzimuthElevationMask_rad;

    for (auto it = anAzimuthElevationMask_deg.begin(); it != anAzimuthElevationMask_deg.end(); ++it)
    {
        anAzimuthElevationMask_rad.insert({it->first * M_PI / 180.0, it->second * M_PI / 180.0});
    }

    this->azimuthElevationMask = anAzimuthElevationMask_rad;
}

bool VisibilityCriterion::AERMask::isSatisfied(const AER& anAer) const
{
    return isSatisfied(
        anAer.getAzimuth().inRadians(0.0, 2.0 * M_PI),
        anAer.getElevation().inRadians(-M_PI, M_PI),
        anAer.getRange().inMeters()
    );
}

bool VisibilityCriterion::AERMask::isSatisfied(
    double anAzimuth_Radians, double anElevation_Radians, double aRange_Meters
) const
{
    double anAzimuthReduced_Radians = Angle(anAzimuth_Radians, Angle::Unit::Radian).inRadians(0.0, 2.0 * M_PI);
    double anElevationReduced_Radians = Angle(anElevation_Radians, Angle::Unit::Radian).inRadians(-M_PI, M_PI);

    auto itLow = this->azimuthElevationMask.lower_bound(anAzimuthReduced_Radians);

    if (itLow->first != anAzimuthReduced_Radians)
    {
        itLow--;
    }
    const auto itUp = this->azimuthElevationMask.upper_bound(anAzimuthReduced_Radians);

    // Vector between the two successive mask data points with bounding azimuth values

    const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

    // Vector from data point with azimuth lower bound to tested point

    const Vector2d lowToPointVector = {
        anAzimuthReduced_Radians - itLow->first, anElevationReduced_Radians - itLow->second
    };

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
