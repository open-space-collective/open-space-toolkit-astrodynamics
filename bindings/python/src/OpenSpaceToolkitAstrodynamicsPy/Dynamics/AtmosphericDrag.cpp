/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_AtmosphericDrag(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::environment::object::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamic::AtmosphericDrag;

    {
        class_<AtmosphericDrag, Dynamics, Shared<AtmosphericDrag>>(
            aModule,
            "AtmosphericDrag",
            R"doc(
                The atmospheric drag dynamics.

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

            .def("__str__", &(shiftToString<AtmosphericDrag>))
            .def("__repr__", &(shiftToString<AtmosphericDrag>))

            .def(
                "is_defined",
                &AtmosphericDrag::isDefined,
                R"doc(
                    Check if the atmospheric drag is defined.

                    Returns:
                        bool: True if the atmospheric drag is defined, False otherwise.

                )doc"
            )

            .def(
                "get_celestial",
                &AtmosphericDrag::getCelestial,
                R"doc(
                    Get the celestial body.

                    Returns:
                        Celestial: The celestial body.

                )doc"
            )

            .def(
                "compute_contribution",
                &AtmosphericDrag::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the atmospheric drag to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the atmospheric drag to the state vector.

                )doc"
            );
    }
}
