/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Polygon.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Custom.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace hardbody
{

using ostk::mathematics::geometry::d2::object::Point;
using ostk::mathematics::geometry::d2::object::Polygon;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Transform;

Custom::Custom(
    const VertexGenerator& aVertexGenerator,
    const std::variant<Shared<const Frame>, LocalOrbitalFrameFactory>& aFrameOrLOFFactory
)
    : HardBody(),
      vertexGenerator_(aVertexGenerator),
      frameOrLOF_(aFrameOrLOFFactory)
{
    if (!vertexGenerator_)
    {
        throw ostk::core::error::runtime::Undefined("Vertex generator");
    }

    if (const Shared<const Frame>* frameSPtr = std::get_if<Shared<const Frame>>(&frameOrLOF_))
    {
        if (*frameSPtr == nullptr)
        {
            throw ostk::core::error::runtime::Undefined("Frame");
        }
    }
    else if (!std::get<LocalOrbitalFrameFactory>(frameOrLOF_).isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }
}

Custom::VertexGenerator Custom::getVertexGenerator() const
{
    return vertexGenerator_;
}

HardBody::CrossSection Custom::calculateCrossSectionAt(const State& aState, const Shared<const Frame>& aFrameSPtr) const
{
    const Array<Vertex> vertices = vertexGenerator_(aState);

    if (vertices.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Vertices");
    }

    const Shared<const Frame> vertexFrameSPtr =
        std::holds_alternative<Shared<const Frame>>(frameOrLOF_)
            ? std::get<Shared<const Frame>>(frameOrLOF_)
            : std::get<LocalOrbitalFrameFactory>(frameOrLOF_).generateFrame(aState);

    // The vertices are offsets from the object center: they are only rotated into the given frame, not translated
    const Transform transform = vertexFrameSPtr->getTransformTo(aFrameSPtr, aState.accessInstant());

    // Projecting along the Z axis onto the X-Y plane drops the Z coordinate
    Array<Point> projectedPoints = Array<Point>::Empty();

    for (const Vertex& vertex : vertices)
    {
        const Vector3d rotatedVertex = transform.applyToVector(vertex.asVector());
        const Point projectedPoint = {rotatedVertex.x(), rotatedVertex.y()};

        if (!projectedPoints.contains(projectedPoint))
        {
            projectedPoints.add(projectedPoint);
        }
    }

    // A polygon requires at least 3 points: fewer distinct points already form a point or a segment
    if (projectedPoints.getSize() < 3)
    {
        return {projectedPoints, 0.0};
    }

    // The polygon convex hull is sorted clockwise and drops duplicate, interior and collinear points
    return {Polygon(projectedPoints).getConvexHull().getVertices(), 0.0};
}

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
