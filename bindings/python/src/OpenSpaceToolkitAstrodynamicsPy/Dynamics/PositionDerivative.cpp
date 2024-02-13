/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_PositionDerivative(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::environment::object::Celestial;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::dynamics::PositionDerivative;

    {
        class_<PositionDerivative, Dynamics, Shared<PositionDerivative>>(
            aModule,
            "PositionDerivative",
            R"doc(
                The position derivative model.

            )doc"
        )
            .def(
                init<>(),
                R"doc(
                    Constructor.

                )doc"
            )

            .def("__str__", &(shiftToString<PositionDerivative>))
            .def("__repr__", &(shiftToString<PositionDerivative>))

            .def(
                "is_defined",
                &PositionDerivative::isDefined,
                R"doc(
                    Check if the position derivative is defined.

                    Returns:
                        bool: True if the position derivative is defined, False otherwise.

                )doc"
            )

            .def(
                "compute_contribution",
                &PositionDerivative::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the position derivative to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the position derivative to the state vector.
                    
                )doc"
            );
    }
}
