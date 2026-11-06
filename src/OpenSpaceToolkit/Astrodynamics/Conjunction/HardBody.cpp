/// Apache License 2.0

#include <algorithm>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Polygon.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::mathematics::geometry::d2::object::Polygon;

HardBody::CrossSection::CrossSection(const Array<Point>& aConvexHull, const Real& aRadius)
    : convexHull_(aConvexHull),
      radius_(aRadius)
{
}

Real HardBody::CrossSection::getEnclosingRadius() const
{
    if (convexHull_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Convex hull");
    }

    // The distance to the origin is convex, so its maximum over the convex hull is reached at a vertex. The
    // dilation then moves the farthest point outwards by the radius.
    Real maximumVertexDistance = 0.0;

    for (const Point& vertex : convexHull_)
    {
        maximumVertexDistance = std::max(maximumVertexDistance, vertex.distanceTo(Point::Origin()));
    }

    return maximumVertexDistance + radius_;
}

HardBody::CrossSection HardBody::CrossSection::combine(
    const CrossSection& aCrossSection, const CrossSection& anotherCrossSection
)
{
    if (aCrossSection.convexHull_.isEmpty() || anotherCrossSection.convexHull_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Convex hull");
    }

    // The Minkowski sum of two convex polygons is the convex hull of the pairwise sums of their vertices
    Array<Point> vertexSums = Array<Point>::Empty();

    for (const Point& vertex : aCrossSection.convexHull_)
    {
        for (const Point& anotherVertex : anotherCrossSection.convexHull_)
        {
            const Point vertexSum = {vertex.x() + anotherVertex.x(), vertex.y() + anotherVertex.y()};

            if (!vertexSums.contains(vertexSum))
            {
                vertexSums.add(vertexSum);
            }
        }
    }

    const Real radius = aCrossSection.radius_ + anotherCrossSection.radius_;

    // A polygon requires at least 3 points: fewer distinct points already form a point or a segment
    if (vertexSums.getSize() < 3)
    {
        return {vertexSums, radius};
    }

    // The polygon convex hull is sorted clockwise and drops duplicate, interior and collinear points
    return {Polygon(vertexSums).getConvexHull().getVertices(), radius};
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
