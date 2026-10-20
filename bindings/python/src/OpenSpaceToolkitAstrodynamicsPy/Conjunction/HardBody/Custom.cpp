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

            Vertices are in meters, relative to the object position. The vertex frame, either a fixed frame or a
            local orbital frame generated from the state, only defines their direction: they are rotated, never
            translated.

            The cross section is the convex hull of the projected vertices, with a zero radius. For a non-convex
            object, it conservatively encloses the actual shadow.
        )doc"
    )

        .def(
            init<const Custom::VertexGenerator&, const Shared<const Frame>&>(),
            R"doc(
                Constructor, with vertices expressed in a frame.

                Args:
                    vertex_generator (Callable[[State], list[Point]]): A function returning the vertices [m] for a
                        given state, relative to the object position and expressed along the vertex frame axes.
                    frame (Frame): A vertex frame.

                Raises:
                    RuntimeError: If the vertex generator or the frame is undefined.

                Examples:
                    >>> # Cube of side 2.0 m, with edges aligned with the GCRF axes
                    >>> def vertex_generator(state: State) -> list[Point]:
                    ...     return [Point(x, y, z) for x in (-1.0, 1.0) for y in (-1.0, 1.0) for z in (-1.0, 1.0)]
                    >>> custom = Custom(vertex_generator=vertex_generator, frame=Frame.GCRF())
            )doc",
            arg("vertex_generator"),
            arg("frame")
        )

        .def(
            init<const Custom::VertexGenerator&, const LocalOrbitalFrameFactory&>(),
            R"doc(
                Constructor, with vertices expressed in a local orbital frame.

                The vertex frame is generated from the state by the factory.

                Args:
                    vertex_generator (Callable[[State], list[Point]]): A function returning the vertices [m] for a
                        given state, relative to the object position and expressed along the vertex frame axes.
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): A local orbital frame factory
                        generating the vertex frame.

                Raises:
                    RuntimeError: If the vertex generator or the factory is undefined.

                Examples:
                    >>> # Same cube, with edges aligned with the VNC axes
                    >>> custom = Custom(
                    ...     vertex_generator=vertex_generator,
                    ...     local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
                    ... )
            )doc",
            arg("vertex_generator"),
            arg("local_orbital_frame_factory")
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

        ;
}
