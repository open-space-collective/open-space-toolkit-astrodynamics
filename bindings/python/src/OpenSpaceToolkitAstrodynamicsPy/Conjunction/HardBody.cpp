/// Apache License 2.0

#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/HardBody/Custom.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/HardBody/Spherical.cpp>

using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::trajectory::State;

// Trampoline class for virtual member functions
class PyHardBody : public HardBody
{
   public:
    using HardBody::HardBody;

    // Trampoline (need one for each virtual function)

    HardBody::CrossSection calculateCrossSectionAt(const State& aState, const Shared<const Frame>& aFrameSPtr)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            HardBody::CrossSection, HardBody, "calculate_cross_section_at", calculateCrossSectionAt, aState, aFrameSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_HardBody(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;

    using ostk::mathematics::geometry::d2::object::Point;

    class_<HardBody, PyHardBody, Shared<HardBody>> hardBody(
        aModule,
        "HardBody",
        R"doc(
            Hard body (abstract).

            Physical extent of an object in conjunction analysis, from which a cross section can be calculated
            (see calculate_cross_section_at).

            Can inherit and provide the virtual method:
                - calculate_cross_section_at
            to create a custom hard body.
        )doc"
    );

    class_<HardBody::CrossSection>(
        hardBody,
        "CrossSection",
        R"doc(
            Cross section of a hard body, i.e. its projection onto a plane.

            A convex hull (convex polygon) dilated by a radius, i.e. all the points lying within the radius of the
            polygon. Coordinates are in meters, relative to the object position: they need no translation.

            The convex hull vertices must be:
                - Sorted clockwise, consistently with ostk.mathematics.geometry.d2.object.Polygon (any vertex first)
                - Not closed: the first vertex is not repeated at the end
                - Vertices of the polygon only: no duplicate, collinear or interior points

            One or two vertices are also allowed (point or segment).

            Examples:
                >>> circle = HardBody.CrossSection([Point(0.0, 0.0)], 1.0)
                >>> square = HardBody.CrossSection(
                ...     [Point(0.5, 0.5), Point(0.5, -0.5), Point(-0.5, -0.5), Point(-0.5, 0.5)], 0.0
                ... )
                >>> stadium = HardBody.CrossSection([Point(-1.0, 0.0), Point(1.0, 0.0)], 0.5)
        )doc"
    )

        .def(
            init<const Array<Point>&, const Real&>(),
            R"doc(
                Constructor.

                Args:
                    convex_hull (list[Point]): The convex hull vertices [m], relative to the object position.
                    radius (float): The radius [m] by which the convex hull is dilated.
            )doc",
            arg("convex_hull"),
            arg("radius")
        )

        .def(
            "get_enclosing_radius",
            &HardBody::CrossSection::getEnclosingRadius,
            R"doc(
                Get the enclosing radius of the cross section.

                Radius of the smallest circle centered on the object position enclosing the cross section: the
                distance to the farthest convex hull vertex, plus the radius.

                Returns:
                    float: The enclosing radius [m].

                Raises:
                    RuntimeError: If the convex hull is empty.
            )doc"
        )

        .def_static(
            "combine",
            &HardBody::CrossSection::combine,
            R"doc(
                Combine two cross sections into their Minkowski sum.

                The convex hulls are Minkowski summed and the radii added, e.g. to obtain the combined cross section
                of the two objects of a close approach.

                Args:
                    cross_section (HardBody.CrossSection): A cross section.
                    another_cross_section (HardBody.CrossSection): Another cross section.

                Returns:
                    HardBody.CrossSection: The combined cross section.

                Raises:
                    RuntimeError: If either convex hull is empty.
            )doc",
            arg("cross_section"),
            arg("another_cross_section")
        )

        ;

    hardBody

        .def(init<>())

        .def(
            "calculate_cross_section_at",
            &HardBody::calculateCrossSectionAt,
            R"doc(
                Calculate the cross section of the hard body at a given state.

                The hard body is projected along the Z axis of the given frame onto its X-Y plane. Only the frame
                orientation is used: the cross section is always relative to the object position. Use the encounter
                frame (see CloseApproach.get_encounter_frame, whose Z axis is the relative velocity) to project onto
                the encounter plane.

                Args:
                    state (State): A state of the object.
                    frame (Frame): A frame whose axes define the projection.

                Returns:
                    HardBody.CrossSection: The cross section.
            )doc",
            arg("state"),
            arg("frame")
        )

        ;

    // Create "hard_body" python submodule
    auto hardBodyModule = aModule.def_submodule("hard_body");

    // Add objects to "hard_body" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_HardBody_Spherical(hardBodyModule);
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_HardBody_Custom(hardBodyModule);
}
