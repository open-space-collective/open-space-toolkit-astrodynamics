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

using ostk::mathematics::object::Vector2d;

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::Object;

Constraint::AERIntervalConstraint::AERIntervalConstraint(
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

    azimuth = Interval<Real>(anAzimuthInterval.getLowerBound() * M_PI/180.0, anAzimuthInterval.getUpperBound() * M_PI/180.0, anAzimuthInterval.getType());
    elevation = Interval<Real>(anElevationInterval.getLowerBound() * M_PI/180.0, anElevationInterval.getUpperBound() * M_PI/180.0, anAzimuthInterval.getType());
}

bool Constraint::AERIntervalConstraint::isSatisfied(const AER& anAer) const
{
    return isSatisfied(anAer.getAzimuth().inRadians(), anAer.getElevation().inRadians(), anAer.getRange().inMeters());
}

bool Constraint::AERIntervalConstraint::isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const
{
    return azimuth.contains(anAzimuthRadians) && elevation.contains(anElevationRadians) &&
           range.contains(aRangeMeters);
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
    return isSatisfied(anAer.getAzimuth().inRadians(0.0, Real::TwoPi()), anAer.getElevation().inRadians(-Real::Pi(), Real::Pi()), anAer.getRange().inMeters());
}

bool Constraint::MaskConstraint::isSatisfied(const Real anAzimuthRadians&, const Real& anElevationRadians, const Real& aRangeMeters) const
{
    auto itLow = this->azimuthElevationMask.lower_bound(anAzimuthRadians);
    itLow--;
    auto itUp = this->azimuthElevationMask.upper_bound(anAzimuthRadians);

    // Vector between the two successive mask data points with bounding azimuth values

    const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

    // Vector from data point with azimuth lower bound to tested point

    const Vector2d lowToPointVector = {anAzimuthRadians - itLow->first, anElevationRadians - itLow->second};

    // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
    // mask

    return ((lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0]) >= 0.0) && aRange.contains(aRangeMeters);
}

Constraint::LineOfSightConstraint::LineOfSightConstraint(const Environment& anEnvironment)
    : environment(anEnvironment)
{
}

bool Constraint::LineOfSightConstraint::isSatisfied(
    const Instant& anInstant, const Vector3d& aFromPositionCoordinates, const Vector3d& aToPositionCoordinates
) const
{
    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    this->environment.setInstant(anInstant);

    const Point fromPositionCoordinates = Point::Vector(aFromPositionCoordinates);
    const Point toPositionCoordinates = Point::Vector(aToPositionCoordinates);

    if (fromPositionCoordinates == toPositionCoordinates)
    {
        return true;
    }

    const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

    const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

    return !this->environment.intersects(fromToSegmentGeometry);
}

Constraint::ElevationIntervalConstraint::ElevationIntervalConstraint(const Interval<Real, Real>& anElevationInterval)
    : elevation(anElevationInterval)
{
    if (elevation.getLowerBound() < -90.0 || elevation.getUpperBound() > 90.0)
    {
        throw ostk::core::error::RuntimeError(
            "The elevation interval [{}, {}] must be in the range [-90, 90] deg.",
            elevation.accessLowerBound(),
            elevation.accessUpperBound()
        );
    }

    elevation = Interval<Real>(anElevationInterval.getLowerBound() * M_PI/180.0, anElevationInterval.getUpperBound() * M_PI/180.0, anAzimuthInterval.getType());
}

bool Constraint::ElevationIntervalConstraint::isSatisfied(const Real& anElevation) const
{
    return elevation.contains(anElevation);
}

Constraint Constraint::FromAERIntervals(
    const Interval<Real>& azimuth, const Interval<Real>& elevation, const Interval<Real>& range
)
{
    return Constraint(AERIntervalConstraint {azimuth, elevation, range});
}

Constraint Constraint::FromMask(const Map<Real, Real>& azimuthElevationMask, const Interval<Real>& range)
{
    return Constraint(MaskConstraint {azimuthElevationMask, range});
}

Constraint Constraint::FromLineOfSight(const Environment& environment)
{
    return Constraint(LineOfSightConstraint {environment});
}

Constraint Constraint::FromElevationInterval(const Interval<Real>& anElevationInterval)
{
    return Constraint(ElevationIntervalConstraint {anElevationInterval});
}

bool Constraint::isAERIntervalBased() const
{
    return std::holds_alternative<AERIntervalConstraint>(constraint_);
}

bool Constraint::isMaskBased() const
{
    return std::holds_alternative<MaskConstraint>(constraint_);
}

bool Constraint::isLineOfSightBased() const
{
    return std::holds_alternative<LineOfSightConstraint>(constraint_);
}

bool Constraint::isElevationIntervalBased() const
{
    return std::holds_alternative<ElevationIntervalConstraint>(constraint_);
}

std::optional<Constraint::AERIntervalConstraint> Constraint::getAERIntervalConstraint() const
{
    if (const auto* aerInterval = std::get_if<AERIntervalConstraint>(&constraint_))
    {
        return *aerInterval;
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

std::optional<Constraint::ElevationIntervalConstraint> Constraint::getElevationIntervalConstraint() const
{
    if (const auto* elevationInterval = std::get_if<ElevationIntervalConstraint>(&constraint_))
    {
        return *elevationInterval;
    }

    return std::nullopt;
}

template <typename T>
Constraint::Constraint(const T& constraint)
    : constraint_(constraint)
{
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
