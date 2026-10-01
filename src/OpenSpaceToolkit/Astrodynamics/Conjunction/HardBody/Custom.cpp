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
        (frameSPtr_ != nullptr) ? frameSPtr_ : localOrbitalFrameFactorySPtr_->generateFrame(aState);

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

Custom Custom::FromInertial(const Shared<const Frame>& aFrameSPtr, const VertexGenerator& aVertexGenerator)
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }

    return {aVertexGenerator, aFrameSPtr, nullptr};
}

Custom Custom::FromLocalOrbitalFrame(
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr, const VertexGenerator& aVertexGenerator
)
{
    if ((aLocalOrbitalFrameFactorySPtr == nullptr) || (!aLocalOrbitalFrameFactorySPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Local orbital frame factory");
    }

    return {aVertexGenerator, nullptr, aLocalOrbitalFrameFactorySPtr};
}

Custom::Custom(
    const VertexGenerator& aVertexGenerator,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
)
    : HardBody(),
      vertexGenerator_(aVertexGenerator),
      frameSPtr_(aFrameSPtr),
      localOrbitalFrameFactorySPtr_(aLocalOrbitalFrameFactorySPtr)
{
    if (!vertexGenerator_)
    {
        throw ostk::core::error::runtime::Undefined("Vertex generator");
    }
}

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
