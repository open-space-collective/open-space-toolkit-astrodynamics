/// Apache License 2.0

#include <algorithm>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

// How far below the surface, in squared scaled radii, a segment must dip before the shortcut below will call it an
// occultation. 1e-6 is a little over three metres of altitude on Earth: far outside floating-point noise, and far
// inside the resolution at which an access boundary is meaningful.
static constexpr double GRAZING_MARGIN_SCALED_SQUARED_NORM = 1.0 - 1e-6;

using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;

VisibilityCriterion::LineOfSight::LineOfSight(const Environment& anEnvironment)
    : environment(anEnvironment)
{
    if (!anEnvironment.hasCentralCelestialObject())
    {
        std::cerr << "Warning: Environment must have a central celestial object for LineOfSight criterion."
                  << std::endl;
    }
}

// Squared distance from the central body's centre to the closest point on the segment, in units of the body's
// own radii: both endpoints arrive in the central body's frame, where its ellipsoid is static and axis-aligned,
// so scaling each axis by its radius maps that ellipsoid to the unit sphere. Below 1 the segment is inside the
// body, above 1 it clears it. A handful of dot products, with no frame transform, no posing of the environment
// and no geometry objects.
static double scaledSquaredDistanceToCentralBody(
    const Vector3d& aFromPositionCoordinates_Body,
    const Vector3d& aToPositionCoordinates_Body,
    const Real& anEquatorialRadius_m,
    const Real& aFlattening,
    double& aFromScaledSquaredNorm,
    double& aToScaledSquaredNorm
)
{
    const double polarRadius_m = anEquatorialRadius_m * (1.0 - aFlattening);

    const Vector3d scaling = {1.0 / anEquatorialRadius_m, 1.0 / anEquatorialRadius_m, 1.0 / polarRadius_m};

    const Vector3d from = aFromPositionCoordinates_Body.cwiseProduct(scaling);
    const Vector3d to = aToPositionCoordinates_Body.cwiseProduct(scaling);

    aFromScaledSquaredNorm = from.squaredNorm();
    aToScaledSquaredNorm = to.squaredNorm();

    const Vector3d delta = to - from;

    const double deltaSquaredNorm = delta.squaredNorm();

    const double parameter = (deltaSquaredNorm > 0.0) ? std::clamp(-from.dot(delta) / deltaSquaredNorm, 0.0, 1.0) : 0.0;

    return (from + parameter * delta).squaredNorm();
}

bool VisibilityCriterion::LineOfSight::isSatisfied(
    const Instant& anInstant, const Vector3d& aFromPositionCoordinates_Body, const Vector3d& aToPositionCoordinates_Body
) const
{
    // TBR: Deprecate this check in a future release
    Shared<const Frame> commonFrameSPtr = nullptr;
    Shared<const Celestial> centralCelestialSPtr = nullptr;
    if (this->environment.hasCentralCelestialObject())
    {
        centralCelestialSPtr = this->environment.accessCentralCelestialObject();
        commonFrameSPtr = centralCelestialSPtr->accessFrame();
    }
    else
    {
        commonFrameSPtr = Frame::ITRF();
    }

    const Point fromPositionCoordinates = Point::Vector(aFromPositionCoordinates_Body);
    const Point toPositionCoordinates = Point::Vector(aToPositionCoordinates_Body);

    if (fromPositionCoordinates == toPositionCoordinates)
    {
        return true;
    }

    // The central body occults most of the segments this is asked about, and it is the one object whose geometry
    // is already expressed in the frame the endpoints arrive in. Answering for it directly avoids posing the whole
    // environment - which evaluates the Sun and Moon ephemerides and re-poses every body's geometry.
    //
    // The shortcut only ever reports an unambiguous occultation, and defers everything else to the general test,
    // so it cannot move an access boundary:
    //
    //   - it requires an endpoint outside the body, because a segment lying wholly within the ellipsoid does not
    //     cross its surface and the general test does not count it as an intersection;
    //   - it requires the segment to dip a clear margin below the surface, so a near-grazing segment - which is
    //     exactly where a crossing is refined, and where a different rounding would shift it - still goes through
    //     the general test.
    if (centralCelestialSPtr != nullptr && centralCelestialSPtr->isDefined())
    {
        double fromScaledSquaredNorm = 0.0;
        double toScaledSquaredNorm = 0.0;

        const double scaledSquaredDistance = scaledSquaredDistanceToCentralBody(
            aFromPositionCoordinates_Body,
            aToPositionCoordinates_Body,
            centralCelestialSPtr->getEquatorialRadius().inMeters(),
            centralCelestialSPtr->getFlattening(),
            fromScaledSquaredNorm,
            toScaledSquaredNorm
        );

        const bool hasEndpointOutside = (fromScaledSquaredNorm > 1.0) || (toScaledSquaredNorm > 1.0);

        if (hasEndpointOutside && (scaledSquaredDistance < GRAZING_MARGIN_SCALED_SQUARED_NORM))
        {
            return false;
        }
    }

    this->environment.setInstant(anInstant);

    const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

    const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

    return !this->environment.intersects(fromToSegmentGeometry);
}

bool VisibilityCriterion::LineOfSight::operator==(const VisibilityCriterion::LineOfSight& aLineOfSight) const
{
    return this->environment.accessObjects() == aLineOfSight.environment.accessObjects();
}

bool VisibilityCriterion::LineOfSight::operator!=(const VisibilityCriterion::LineOfSight& aLineOfSight) const
{
    return !(*this == aLineOfSight);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
