/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_ThirdBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::environment::object::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::ThirdBodyGravity;

    {
        class_<ThirdBodyGravity, Dynamics, Shared<ThirdBodyGravity>>(
            aModule,
            "ThirdBodyGravity",
            R"doc(
                The third body gravity model.

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

            .def("__str__", &(shiftToString<ThirdBodyGravity>))
            .def("__repr__", &(shiftToString<ThirdBodyGravity>))

            .def(
                "is_defined",
                &ThirdBodyGravity::isDefined,
                R"doc(
                    Check if the third-body gravity is defined.

                    Returns:
                        bool: True if the third-body gravity is defined, False otherwise.

                )doc"
            )

            .def(
                "get_celestial",
                &ThirdBodyGravity::getCelestial,
                R"doc(
                    Get the celestial body.

                    Returns:
                        Celestial: The celestial body.

                )doc"
            )

            .def(
                "compute_contribution",
                &ThirdBodyGravity::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the third-body gravity to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the third-body gravity to the state vector.

                )doc"
            );
    }
}
