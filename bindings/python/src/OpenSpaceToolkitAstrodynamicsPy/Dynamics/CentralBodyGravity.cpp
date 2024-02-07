/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_CentralBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::environment::object::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::CentralBodyGravity;

    {
        class_<CentralBodyGravity, Dynamics, Shared<CentralBodyGravity>>(
            aModule,
            "CentralBodyGravity",
            R"doc(
                The central-body gravity model.

            )doc"
        )
            .def(
                init<const Shared<Celestial>&>(),
                arg("celestial"),
                R"doc(
                    Constructor.

                    Args:
                        celestial (Celestial): The central body.

                )doc"
            )

            .def("__str__", &(shiftToString<CentralBodyGravity>))
            .def("__repr__", &(shiftToString<CentralBodyGravity>))

            .def(
                "is_defined",
                &CentralBodyGravity::isDefined,
                R"doc(
                    Check if the central-body gravity is defined.

                    Returns:
                        bool: True if the central-body gravity is defined, False otherwise.

                )doc"
            )

            .def(
                "get_celestial",
                &CentralBodyGravity::getCelestial,
                R"doc(
                    Get the central body.

                    Returns:
                        Celestial: The central body.

                )doc"
            )

            .def(
                "compute_contribution",
                &CentralBodyGravity::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the central-body gravity to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the central-body gravity to the state vector.

                )doc"
            );
    }
}
