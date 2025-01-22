/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Maneuver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::astrodynamics::flight::Maneuver;

    class_<Maneuver>(
        aModule,
        "Maneuver",
        R"doc(
            Spacecraft Maneuver class.
            Store an acceleration and mass flow rate profile of a spacecraft maneuver.
        )doc"
    )

        .def(
            init<const Array<State>&>(),
            arg("states"),
            R"doc(
                Constructor.

                Args:
                    states (list[State]): An list of states, must be sorted, must include the CartesianPosition, CartesianVelocity, NewtonianAcceleration and MassFlowRate subsets.
            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Maneuver>))
        .def("__repr__", &(shiftToString<Maneuver>))

        .def(
            "is_defined",
            &Maneuver::isDefined,
            R"doc(
                Check if the maneuver is defined.

                Returns:
                    bool: True if the maneuver is defined, False otherwise. (Always returns true).
            )doc"
        )
        .def(
            "get_states",
            &Maneuver::getStates,
            R"doc(
                Get the states.

                Returns:
                    list[State]: The states.
            )doc"
        )
        .def(
            "get_interval",
            &Maneuver::getInterval,
            R"doc(
                Get the interval of the maneuver.

                Returns:
                    Interval: The interval.
            )doc"
        )
        .def(
            "calculate_delta_v",
            &Maneuver::calculateDeltaV,
            R"doc(
                Calculate the delta-v of the maneuver.

                Returns:
                    float: The delta-v value (m/s).
            )doc"
        )
        .def(
            "calculate_delta_mass",
            &Maneuver::calculateDeltaMass,
            R"doc(
                Calculate the delta mass of the maneuver.

                Returns:
                    Mass: The delta mass (always positive) (kg).
            )doc"
        )
        .def(
            "calculate_average_thrust",
            &Maneuver::calculateAverageThrust,
            arg("initial_spacecraft_mass"),
            R"doc(
                Calculate the average thrust of the maneuver.

                Args:
                    initial_spacecraft_mass (Mass): The initial mass of the spacecraft.

                Returns:
                    float: The average thrust (N).
            )doc"
        )
        .def(
            "calculate_average_specific_impulse",
            &Maneuver::calculateAverageSpecificImpulse,
            arg("initial_spacecraft_mass"),
            R"doc(
                Calculate the average specific impulse of the maneuver.

                Args:
                    initial_spacecraft_mass (Mass): The initial mass of the spacecraft.

                Returns:
                    float: The average specific impulse (s).
            )doc"
        )
        .def(
            "to_tabulated_dynamics",
            &Maneuver::toTabulatedDynamics,
            arg_v("frame", Maneuver::DefaultAccelFrameSPtr, "GCRF"),
            arg("interpolation_type") = DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Convert the maneuver to tabulated dynamics.

                Args:
                    frame (Frame, optional): The frame in which the acceleration profile is defined. Defaults to the default acceleration frame.
                    interpolation_type (Interpolator.Type, optional): The interpolation type to use. Defaults to the default interpolation type.

                Returns:
                    Tabulated: The tabulated dynamics.
            )doc"
        )
        .def_static(
            "constant_mass_flow_rate_profile",
            &Maneuver::ConstantMassFlowRateProfile,
            arg("states"),
            arg("mass_flow_rate"),
            R"doc(
                Create a maneuver from a constant mass flow rate profile.

                Args:
                    instants (list[Instant]): An array of instants, must be sorted.
                    acceleration_profile (list[numpy.ndarray]): An acceleration profile of the maneuver, one numpy.ndarray per instant.
                    frame (Frame): A frame in which the acceleration profile is defined.
                    mass_flow_rate (float): The constant mass flow rate (negative number expected).

                Returns:
                    Maneuver: The created maneuver.
            )doc"
        );
}
