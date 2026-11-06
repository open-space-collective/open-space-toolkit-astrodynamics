/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_HardBody_Spherical(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::astrodynamics::conjunction::HardBody;
    using ostk::astrodynamics::conjunction::hardbody::Spherical;

    class_<Spherical, HardBody, Shared<Spherical>>(
        aModule,
        "Spherical",
        R"doc(
            Spherical hard body.

            A sphere centered on the object position. Its cross section is a circle of the same radius, whatever the
            state or frame.
        )doc"
    )

        .def(
            init<const Real&>(),
            R"doc(
                Constructor.

                Args:
                    radius (float): The radius of the sphere [m].

                Raises:
                    RuntimeError: If the radius is undefined or negative.
            )doc",
            arg("radius")
        )

        .def(
            "get_radius",
            &Spherical::getRadius,
            R"doc(
                Get the radius of the sphere.

                Returns:
                    float: The radius of the sphere [m].
            )doc"
        )

        .def(
            "calculate_cross_section_at",
            &Spherical::calculateCrossSectionAt,
            R"doc(
                Calculate the cross section of the sphere at a given state.

                A circle of the sphere radius, i.e. a single vertex at the object position dilated by the radius.

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
}
