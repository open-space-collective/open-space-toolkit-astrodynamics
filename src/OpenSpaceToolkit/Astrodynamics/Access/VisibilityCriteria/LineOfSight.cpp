/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriteria.hpp>

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

VisibilityCriteria::LineOfSight::LineOfSight(const Environment& anEnvironment)
    : environment(anEnvironment)
{
}

bool VisibilityCriteria::LineOfSight::isSatisfied(
    const Instant& anInstant, const Vector3d& aFromPositionCoordinates_ITRF, const Vector3d& aToPositionCoordinates_ITRF
) const
{
    static const Shared<const Frame> commonFrameSPtr = Frame::ITRF();

    this->environment.setInstant(anInstant);

    const Point fromPositionCoordinates = Point::Vector(aFromPositionCoordinates_ITRF);
    const Point toPositionCoordinates = Point::Vector(aToPositionCoordinates_ITRF);

    if (fromPositionCoordinates == toPositionCoordinates)
    {
        return true;
    }

    const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

    const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

    return !this->environment.intersects(fromToSegmentGeometry);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
