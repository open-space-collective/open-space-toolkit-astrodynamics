/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Nadir.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Nadir(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::time::Instant;
    using ostk::physics::time::Duration;

    using ostk::astrodynamics::trajectory::model::Nadir;
    using ostk::astrodynamics::trajectory::Orbit;
    using ostk::astrodynamics::trajectory::State;

    class_<Nadir, ostk::astrodynamics::trajectory::Model>(
        aModule,
        "Nadir",
        R"doc(
            Nadir trajectory model.

            This model represents a trajectory that follows the nadir direction of an orbit.
        )doc"
    )

        .def(
            init<const Orbit&, const Duration&>(),
            R"doc(
                Construct a `Nadir` object from an orbit.

                Args:
                    orbit (Orbit): The orbit.
                    step_size (Duration): The step size for the trajectory. Defaults to 1e-2 seconds.

                Returns:
                    Nadir: The `Nadir` object.
            )doc",
            arg("orbit"),
            arg("step_size") = Duration::Seconds(1e-2)
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Nadir>))
        .def("__repr__", &(shiftToString<Nadir>))

        .def(
            "is_defined",
            &Nadir::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
            )doc"
        )
        .def(
            "calculate_state_at",
            &Nadir::calculateStateAt,
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.
            )doc",
            arg("instant")
        )
        .def(
            "get_orbit",
            &Nadir::getOrbit,
            R"doc(
                Get the orbit of the nadir model.

                Returns:
                    Orbit: The orbit of the nadir model.
            )doc"
        )
        .def(
            "get_step_size",
            &Nadir::getStepSize,
            R"doc(
                Get the step size of the nadir model.

                Returns:
                    Duration: The step size of the nadir model.
            )doc"
        )

        ;
}
