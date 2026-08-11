/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AnalyticalThirdBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_AnalyticalThirdBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::physics::environment::object::Celestial;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::dynamics::AnalyticalThirdBodyGravity;

    {
        class_<AnalyticalThirdBodyGravity, Dynamics, Shared<AnalyticalThirdBodyGravity>>(
            aModule,
            "AnalyticalThirdBodyGravity",
            R"doc(
                The analytical third body gravity model.

                The third body is treated as a point mass, using the gravitational parameter of the
                celestial object.

                Unlike `ThirdBodyGravity`, the position of the third body is obtained from
                `Celestial.compute_analytical_position` instead of from the ephemeris of the celestial object.
                This is orders of magnitude faster than a SPICE-based ephemeris, at the cost of a lower
                position accuracy (< 0.1 deg in direction).

                Only supports the celestial objects providing an analytical model (Sun and Moon).

            )doc"
        )
            .def(
                init<const Shared<Celestial>&>(),
                arg("celestial"),
                R"doc(
                    Constructor.

                    Args:
                        celestial (Celestial): The celestial body.

                )doc"
            )

            .def(
                init<const Shared<Celestial>&, const String&>(),
                arg("celestial"),
                arg("name"),
                R"doc(
                    Constructor.

                    Args:
                        celestial (Celestial): The celestial body.
                        name (str): The name of the dynamics.

                )doc"
            )

            .def("__str__", &(shiftToString<AnalyticalThirdBodyGravity>))
            .def("__repr__", &(shiftToString<AnalyticalThirdBodyGravity>))

            .def(
                "is_defined",
                &AnalyticalThirdBodyGravity::isDefined,
                R"doc(
                    Check if the analytical third-body gravity is defined.

                    Returns:
                        bool: True if the analytical third-body gravity is defined, False otherwise.

                )doc"
            )

            .def(
                "get_celestial",
                &AnalyticalThirdBodyGravity::getCelestial,
                R"doc(
                    Get the celestial body.

                    Returns:
                        Celestial: The celestial body.

                )doc"
            )

            .def(
                "compute_contribution",
                &AnalyticalThirdBodyGravity::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the analytical third-body gravity to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the analytical third-body gravity to the state vector.

                )doc"
            );
    }
}
