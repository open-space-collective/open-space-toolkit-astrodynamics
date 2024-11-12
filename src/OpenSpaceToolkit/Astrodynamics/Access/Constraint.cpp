/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Constraint.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::Object;

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

    this->azimuthElevationMask = anAzimuthElevationMask_rad;
}

bool Constraint::MaskConstraint::isSatisfied(const AER& anAer) const
{
    const Real azimuth_rad = anAer.getAzimuth().inRadians(0.0, Real::TwoPi());
    const Real elevation_rad = anAer.getElevation().inRadians(-Real::Pi(), Real::Pi());

    auto itLow = this->azimuthElevationMask.lower_bound(azimuth_rad);
    itLow--;
    auto itUp = this->azimuthElevationMask.upper_bound(azimuth_rad);

    // Vector between the two successive mask data points with bounding azimuth values

    const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

    // Vector from data point with azimuth lower bound to tested point

    const Vector2d lowToPointVector = {azimuth_rad - itLow->first, elevation_rad - itLow->second};

    // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
    // mask

    return ((lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0]) >= 0.0);
}

Constraint::LineOfSightConstraint::LineOfSightConstraint(const Environment& anEnvironment)
    : environment(anEnvironment)
{
}

bool Constraint::LineOfSightConstraint::isSatisfied(
    const Instant& anInstant, const Position& aFromPosition, const Position& aToPosition
) const
{
    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    this->environment.setInstant(anInstant);

    const Point fromPositionCoordinates = Point::Vector(aFromPosition.accessCoordinates());
    const Point toPositionCoordinates = Point::Vector(aToPosition.accessCoordinates());

    if (fromPositionCoordinates == toPositionCoordinates)
    {
        return true;
    }

    const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

    const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

    return !this->environment.intersects(fromToSegmentGeometry);
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

Constraint Constraint::FromLineOfSight(const Environment& environment)
{
    return Constraint(LineOfSightConstraint {environment});
}

bool Constraint::isMaskBased() const
{
    return std::holds_alternative<MaskConstraint>(constraint_);
}

bool Constraint::isIntervalBased() const
{
    return std::holds_alternative<IntervalConstraint>(constraint_);
}

bool Constraint::isLineOfSightBased() const
{
    return std::holds_alternative<LineOfSightConstraint>(constraint_);
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

std::optional<Constraint::LineOfSightConstraint> Constraint::getLineOfSightConstraint() const
{
    if (const auto* lineOfSight = std::get_if<LineOfSightConstraint>(&constraint_))
    {
        return *lineOfSight;
    }

    return std::nullopt;
}

Constraint::Constraint(const IntervalConstraint& constraint)
    : constraint_(constraint)
{
}

Constraint::Constraint(const MaskConstraint& constraint)
    : constraint_(constraint)
{
}

Constraint::Constraint(const LineOfSightConstraint& constraint)
    : constraint_(constraint)
{
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
