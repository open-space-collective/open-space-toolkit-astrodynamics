/// Apache License 2.0

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Custom.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_HardBody_Custom(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::conjunction::HardBody;
    using ostk::astrodynamics::conjunction::hardbody::Custom;
    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

    class_<Custom, HardBody, Shared<Custom>>(
        aModule,
        "Custom",
        R"doc(
            Custom hard body.

            The convex hull of a set of 3D vertices returned by a user-defined generator, which may depend on the
            state (e.g. to account for the attitude).

            Vertices are in meters, relative to the object position. The vertex frame, either an inertial frame (see
            from_inertial) or a local orbital frame (see from_local_orbital_frame), only defines their direction:
            they are rotated, never translated.

            The cross section is the convex hull of the projected vertices, with a zero radius. For a non-convex
            object, it conservatively encloses the actual shadow.
        )doc"
    )

        .def(
            "get_vertex_generator",
            &Custom::getVertexGenerator,
            R"doc(
                Get the vertex generator.

                Returns:
                    Callable[[State], list[Point]]: The vertex generator.
            )doc"
        )

        .def(
            "calculate_cross_section_at",
            &Custom::calculateCrossSectionAt,
            R"doc(
                Calculate the cross section of the hard body at a given state.

                The vertices are rotated from the vertex frame into the given frame, then projected.

                Args:
                    state (State): A state of the object.
                    frame (Frame): A frame whose axes define the projection.

                Returns:
                    HardBody.CrossSection: The cross section.

                Raises:
                    RuntimeError: If the vertex generator returns no vertex.
            )doc",
            arg("state"),
            arg("frame")
        )

        .def_static(
            "from_inertial",
            &Custom::FromInertial,
            R"doc(
                Construct a custom hard body with vertices expressed in an inertial frame.

                Args:
                    frame (Frame): An inertial (or quasi-inertial) vertex frame.
                    vertex_generator (Callable[[State], list[Point]]): A function returning the vertices [m] for a
                        given state, relative to the object position and expressed along the vertex frame axes.

                Returns:
                    Custom: A custom hard body.

                Raises:
                    RuntimeError: If the frame or the vertex generator is undefined, or if the frame is not
                        quasi-inertial.

                Examples:
                    >>> # Cube of side 2.0 m, with edges aligned with the GCRF axes
                    >>> def vertex_generator(state: State) -> list[Point]:
                    ...     return [Point(x, y, z) for x in (-1.0, 1.0) for y in (-1.0, 1.0) for z in (-1.0, 1.0)]
                    >>> custom = Custom.from_inertial(frame=Frame.GCRF(), vertex_generator=vertex_generator)
            )doc",
            arg("frame"),
            arg("vertex_generator")
        )

        .def_static(
            "from_local_orbital_frame",
            &Custom::FromLocalOrbitalFrame,
            R"doc(
                Construct a custom hard body with vertices expressed in a local orbital frame.

                The vertex frame is generated from the state by the factory.

                Args:
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): A local orbital frame factory
                        generating the vertex frame.
                    vertex_generator (Callable[[State], list[Point]]): A function returning the vertices [m] for a
                        given state, relative to the object position and expressed along the vertex frame axes.

                Returns:
                    Custom: A custom hard body.

                Raises:
                    RuntimeError: If the factory or the vertex generator is undefined.

                Examples:
                    >>> # Same cube, with edges aligned with the VNC axes
                    >>> custom = Custom.from_local_orbital_frame(
                    ...     local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
                    ...     vertex_generator=vertex_generator,
                    ... )
            )doc",
            arg("local_orbital_frame_factory"),
            arg("vertex_generator")
        )

        ;
}
