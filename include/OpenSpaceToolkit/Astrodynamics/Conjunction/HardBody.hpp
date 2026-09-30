/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Point.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::geometry::d2::object::Point;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::State;

/// @brief Hard body (abstract).
///
/// @details Physical extent of an object in conjunction analysis, from which a cross section can be calculated
/// (see calculateCrossSectionAt).
class HardBody
{
   public:
    /// @brief Cross section of a hard body, i.e. its projection onto a plane.
    ///
    /// @details A convex hull (convex polygon) dilated by a radius, i.e. all the points lying within the radius
    /// of the polygon. Coordinates are in meters, relative to the object position: they need no translation.
    ///
    /// The convex hull vertices must be:
    /// - Sorted clockwise, consistently with ostk::mathematics::geometry::d2::object::Polygon (any vertex first)
    /// - Not closed: the first vertex is not repeated at the end
    /// - Vertices of the polygon only: no duplicate, collinear or interior points
    ///
    /// One or two vertices are also allowed (point or segment).
    ///
    /// @code{.cpp}
    ///              // Circle of radius 1.0 m
    ///              HardBody::CrossSection circle = { Array<Point>({ { 0.0, 0.0 } }), 1.0 } ;
    ///
    ///              // Square of side 1.0 m
    ///              HardBody::CrossSection square = {
    ///                  Array<Point>({ { 0.5, 0.5 }, { 0.5, -0.5 }, { -0.5, -0.5 }, { -0.5, 0.5 } }), 0.0
    ///              } ;
    ///
    ///              // Stadium: segment of length 2.0 m dilated by a radius of 0.5 m (total size 3.0 m x 1.0 m)
    ///              HardBody::CrossSection stadium = { Array<Point>({ { -1.0, 0.0 }, { 1.0, 0.0 } }), 0.5 } ;
    /// @endcode
    class CrossSection
    {
       public:
        /// @brief Constructor
        ///
        /// @param aConvexHull The convex hull vertices [m], relative to the object position (see class
        /// documentation)
        /// @param aRadius The radius [m] by which the convex hull is dilated
        CrossSection(const Array<Point>& aConvexHull, const Real& aRadius);

        /// @brief Get the enclosing radius of the cross section
        ///
        /// @details Radius of the smallest circle centered on the object position enclosing the cross section:
        /// the distance to the farthest convex hull vertex, plus the radius. Raises an Undefined error if the
        /// convex hull is empty.
        ///
        /// @return The enclosing radius [m]
        Real getEnclosingRadius() const;

        /// @brief Combine two cross sections into their Minkowski sum
        ///
        /// @details The convex hulls are Minkowski summed and the radii added, e.g. to obtain the combined cross
        /// section of the two objects of a close approach. Raises an Undefined error if either convex hull is
        /// empty.
        ///
        /// @code{.cpp}
        ///              // Square of side 1.0 m with corners rounded by 1.0 m
        ///              HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(circle, square) ;
        /// @endcode
        ///
        /// @param aCrossSection A cross section
        /// @param anotherCrossSection Another cross section
        /// @return The combined cross section
        static CrossSection combine(const CrossSection& aCrossSection, const CrossSection& anotherCrossSection);

       private:
        Array<Point> convexHull_;
        Real radius_;
    };

    /// @brief Destructor
    virtual ~HardBody() = default;

    /// @brief Calculate the cross section of the hard body at a given state
    ///
    /// @details The hard body is projected along the Z axis of the given frame onto its X-Y plane. Only the frame
    /// orientation is used: the cross section is always relative to the object position. Use the encounter frame
    /// (see CloseApproach::getEncounterFrame, whose Z axis is the relative velocity) to project onto the
    /// encounter plane.
    ///
    /// @code{.cpp}
    ///              Shared<const Frame> encounterFrame = closeApproach.getEncounterFrame() ;
    ///              HardBody::CrossSection crossSection = hardBody.calculateCrossSectionAt(state, encounterFrame) ;
    /// @endcode
    ///
    /// @param aState A state of the object
    /// @param aFrameSPtr A frame whose axes define the projection
    /// @return The cross section
    virtual CrossSection calculateCrossSectionAt(const State& aState, const Shared<const Frame>& aFrameSPtr) const = 0;
};

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
