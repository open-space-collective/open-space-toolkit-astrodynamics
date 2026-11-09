/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Maneuver(nanobind::module_& aModule)
{
    using namespace nanobind;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::physics::unit::Angle;

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
            init<const Array<State>&, const Interpolator::Type&>(),
            arg("states"),
            arg("interpolation_type").sig("Interpolator.Type.BarycentricRational") =
                DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Constructor.

                Args:
                    states (list[State]): An list of states, must be sorted, must include the CartesianPosition, CartesianVelocity, CartesianAcceleration and MassFlowRate subsets.
                    interpolation_type (Interpolator.Type, optional): The interpolation type used to interpolate the maneuver states (e.g. when converting it to tabulated dynamics to propagate through them). Defaults to Interpolator.Type.BarycentricRational.
            )doc"
        )

        .def(
            "__eq__",
            [](const Maneuver& self, const Maneuver& other)
            {
                return self == other;
            },
            is_operator()
        )
        .def(
            "__ne__",
            [](const Maneuver& self, const Maneuver& other)
            {
                return self != other;
            },
            is_operator()
        )

        .def("__str__", &(shiftToString<Maneuver>))
        .def("__repr__", &(shiftToString<Maneuver>))

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
            "get_interpolation_type",
            &Maneuver::getInterpolationType,
            R"doc(
                Get the interpolation type.

                Returns:
                    Interpolator.Type: The interpolation type.
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
            arg("frame").sig("GCRF") = Maneuver::DefaultAccelFrameSPtr,
            R"doc(
                Convert the maneuver to tabulated dynamics, which use the interpolation type of the maneuver.

                Args:
                    frame (Frame, optional): The frame in which the acceleration profile is defined. Defaults to the default acceleration frame.

                Returns:
                    Tabulated: The tabulated dynamics.
            )doc"
        )
        .def(
            "calculate_mean_thrust_direction_and_maximum_angular_offset",
            &Maneuver::calculateMeanThrustDirectionAndMaximumAngularOffset,
            arg("local_orbital_frame_factory"),
            R"doc(
                Calculate the mean thrust direction in the Local Orbital Frame and its maximum angular offset w.r.t. the maneuver's thrust acceleration directions.
            
                Args:
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): The local orbital frame factory.

                Returns:
                    Tuple[LocalOrbitalFrameDirection, Angle]: The mean thrust direction and its maximum angular offset.
            )doc"
        )
        .def(
            "to_constant_local_orbital_frame_direction_maneuver",
            &Maneuver::toConstantLocalOrbitalFrameDirectionManeuver,
            arg("local_orbital_frame_factory"),
            arg("maximum_allowed_angular_offset") = none(),
            R"doc(
                Create a maneuver with a constant thrust acceleration direction in the Local Orbital Frame.

                The new Maneuver contains the same states as the original Maneuver, but the thrust acceleration direction is 
                constant in the Local Orbital Frame. Said direction is the mean direction of the thrust acceleration directions 
                in the Local Orbital Frame of the original Maneuver. The thrust acceleration magnitude profile is the same as the original.

                If provided, a runtime error will be thrown if the maximum allowed angular offset between the original thrust acceleration direction 
                and the mean thrust direction is violated.

                Args:
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): The local orbital frame factory.
                    maximum_allowed_angular_offset (Angle | None, optional): The maximum allowed angular offset to consider (if any). Defaults to None.

                Returns:
                    Maneuver: The constant local orbital frame direction maneuver.
            )doc"
        )
        .def_static(
            "constant_mass_flow_rate_profile",
            &Maneuver::ConstantMassFlowRateProfile,
            arg("states"),
            arg("mass_flow_rate"),
            arg("interpolation_type").sig("Interpolator.Type.BarycentricRational") =
                DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Create a maneuver from a constant mass flow rate profile.

                Args:
                    states (list[State]): An list of states, must be sorted, must include the CartesianPosition, CartesianVelocity and CartesianAcceleration subsets.
                    mass_flow_rate (float): The constant mass flow rate (negative number expected).
                    interpolation_type (Interpolator.Type, optional): The interpolation type of the maneuver. Defaults to Interpolator.Type.BarycentricRational.

                Returns:
                    Maneuver: The created maneuver.
            )doc"
        );
}
