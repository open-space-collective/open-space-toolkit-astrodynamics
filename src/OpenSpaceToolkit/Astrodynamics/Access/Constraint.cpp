/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/Constraint.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

Constraint::IntervalConstraint::IntervalConstraint(
    const Interval<Real>& anAzimuthInterval,
    const Interval<Real>& anElevationInterval,
    const Interval<Real>& aRangeInterval
)
    : azimuth(anAzimuthInterval),
      elevation(anElevationInterval),
      range(aRangeInterval)
{
    if (!azimuth.isDefined() || !elevation.isDefined() || !range.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    if (azimuth.getLowerBound() < 0.0 || azimuth.getUpperBound() > 360.0)
    {
        throw ostk::core::error::RuntimeError(
            "The azimuth interval [{}, {}] must be in the range [0, 360] deg",
            azimuth.accessLowerBound(),
            azimuth.accessUpperBound()
        );
    }

    if (elevation.getLowerBound() < -90.0 || elevation.getUpperBound() > 90.0)
    {
        throw ostk::core::error::RuntimeError(
            "The elevation interval [{}, {}] must be in the range [-90, 90] deg.",
            elevation.accessLowerBound(),
            elevation.accessUpperBound()
        );
    }

    if (range.getLowerBound() < 0.0)
    {
        throw ostk::core::error::RuntimeError(
            "The range interval [{}, {}] must be positive.", range.accessLowerBound(), range.accessUpperBound()
        );
    }
}

bool Constraint::IntervalConstraint::isSatisfied(const AER& anAer) const
{
    return azimuth.contains(anAer.getAzimuth().inDegrees()) && elevation.contains(anAer.getElevation().inDegrees()) &&
           range.contains(anAer.getRange().inMeters());
}

Constraint::MaskConstraint::MaskConstraint(
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
        throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if ((azimuthElevationPair.second).abs() > 90.0)
        {
            throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
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

    azimuthElevationMask = anAzimuthElevationMask_rad;
}

bool Constraint::MaskConstraint::isSatisfied(const AER& anAer) const
{
    const Real azimuth_rad = anAer.getAzimuth().inRadians(0.0, Real::TwoPi());
    const Real elevation_rad = anAer.getElevation().inRadians(-Real::Pi(), Real::Pi());

    auto itLow = azimuthElevationMask.lower_bound(azimuth_rad);
    itLow--;
    auto itUp = azimuthElevationMask.upper_bound(azimuth_rad);

    // Vector between the two successive mask data points with bounding azimuth values

    const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

    // Vector from data point with azimuth lower bound to tested point

    const Vector2d lowToPointVector = {azimuth_rad - itLow->first, elevation_rad - itLow->second};

    // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
    // mask

    return ((lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0]) >= 0.0);
}

Constraint Constraint::FromIntervals(
    const Interval<Real>& azimuth, const Interval<Real>& elevation, const Interval<Real>& range
)
{
    return Constraint(IntervalConstraint {azimuth, elevation, range});
}

Constraint Constraint::FromMask(const Map<Real, Real>& azimuthElevationMask, const Interval<Real>& range)
{
    return Constraint(MaskConstraint {azimuthElevationMask, range});
}

bool Constraint::isSatisfied(const AER& aer) const
{
    return std::visit(
        [&aer](const auto& constraint)
        {
            return constraint.isSatisfied(aer);
        },
        constraint_
    );
}

bool Constraint::isMaskBased() const
{
    return std::holds_alternative<MaskConstraint>(constraint_);
}

bool Constraint::isIntervalBased() const
{
    return std::holds_alternative<IntervalConstraint>(constraint_);
}

std::optional<Constraint::IntervalConstraint> Constraint::getIntervalConstraint() const
{
    if (const auto* interval = std::get_if<IntervalConstraint>(&constraint_))
    {
        return *interval;
    }
    return std::nullopt;
}

std::optional<Constraint::MaskConstraint> Constraint::getMaskConstraint() const
{
    if (const auto* mask = std::get_if<MaskConstraint>(&constraint_))
    {
        return *mask;
    }
    return std::nullopt;
}

Interval<Real> Constraint::getRangeInterval() const
{
    return std::visit(
        [](const auto& constraint) -> Interval<Real>
        {
            return constraint.range;
        },
        constraint_
    );
}

Constraint::Constraint(IntervalConstraint constraint)
    : constraint_(std::move(constraint))
{
}

Constraint::Constraint(MaskConstraint constraint)
    : constraint_(std::move(constraint))
{
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
