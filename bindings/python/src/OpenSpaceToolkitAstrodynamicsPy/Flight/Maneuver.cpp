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
            Stores information and metadata about a finite maneuver that will be or has been performed.
        )doc"
    )

        .def(
            init<const Array<Instant>&, const Array<Vector3d>&, const Shared<const Frame>&, const Array<Real>&>(),
            arg("instants"),
            arg("acceleration_profile"),
            arg("frame"),
            arg("mass_flow_rate_profile"),
            R"doc(
                Constructor.

                Args:
                    instants (list[Instant]): An array of instants, must be sorted.
                    acceleration_profile (list[numpy.ndarray]): An acceleration profile of the maneuver, one numpy.ndarray per instant.
                    frame (Frame): A frame in which the acceleration profile is defined.
                    mass_flow_rate_profile (list[float]):  A mass flow rate profile of the maneuver, one float per instant.
            )doc"
        )

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
            "get_instants",
            &Maneuver::getInstants,
            R"doc(
                Get the instants.

                Returns:
                    list[Instant]: The instants.
            )doc"
        )
        .def(
            "get_acceleration_profile",
            &Maneuver::getAccelerationProfile,
            arg("frame") = Maneuver::DefaultAccelFrameSPtr,
            R"doc(
                Get the acceleration profile.

                Args:
                    frame (Frame, optional): The frame in which the acceleration profile is defined. Defaults to the default acceleration frame.

                Returns:
                    list[numpy.ndarray]: The acceleration profile.
            )doc"
        )
        .def(
            "get_mass_flow_rate_profile",
            &Maneuver::getMassFlowRateProfile,
            R"doc(
                Get the mass flow rate profile.

                Returns:
                    list[float]: The mass flow rate profile.
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
                    Scalar: The delta-v value.
            )doc"
        )
        .def(
            "calculate_delta_mass",
            &Maneuver::calculateDeltaMass,
            R"doc(
                Calculate the delta mass of the maneuver.

                Returns:
                    Mass: The delta mass (always positive).
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
                    Scalar: The average thrust.
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
                    Scalar: The average specific impulse.
            )doc"
        )
        .def(
            "calculate_acceleration_at",
            &Maneuver::calculateAccelerationAt,
            arg("instant"),
            arg("frame") = Maneuver::DefaultAccelFrameSPtr,
            arg("interpolation_type") = DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Calculate the acceleration at a specific instant.

                Args:
                    instant (Instant): The instant at which to calculate the acceleration.
                    frame (Frame, optional): The frame in which to calculate the acceleration. Defaults to the default acceleration frame.
                    interpolation_type (Interpolator.Type, optional): The interpolation type to use. Defaults to the default interpolation type.

                Returns:
                    numpy.ndarray: The acceleration vector.
            )doc"
        )
        .def(
            "calculate_accelerations_at",
            &Maneuver::calculateAccelerationsAt,
            arg("instants"),
            arg("frame") = Maneuver::DefaultAccelFrameSPtr,
            arg("interpolation_type") = DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Calculate the accelerations at multiple instants.

                Args:
                    instants (list[Instant]): The instants at which to calculate the accelerations.
                    frame (Frame, optional): The frame in which to calculate the accelerations. Defaults to the default acceleration frame.
                    interpolation_type (Interpolator.Type, optional): The interpolation type to use. Defaults to the default interpolation type.

                Returns:
                    list[numpy.ndarray]: The acceleration vectors.
            )doc"
        )
        .def(
            "calculate_mass_flow_rate_at",
            &Maneuver::calculateMassFlowRateAt,
            arg("instant"),
            arg("interpolation_type") = DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Calculate the mass flow rate at a specific instant.

                Args:
                    instant (Instant): The instant at which to calculate the mass flow rate.
                    interpolation_type (Interpolator.Type, optional): The interpolation type to use. Defaults to the default interpolation type.

                Returns:
                    float: The mass flow rate.
            )doc"
        )
        .def(
            "calculate_mass_flow_rates_at",
            &Maneuver::calculateMassFlowRatesAt,
            arg("instants"),
            arg("interpolation_type") = DEFAULT_MANEUVER_INTERPOLATION_TYPE,
            R"doc(
                Calculate the mass flow rates at multiple instants.

                Args:
                    instants (list[Instant]): The instants at which to calculate the mass flow rates.
                    interpolation_type (Interpolator.Type, optional): The interpolation type to use. Defaults to the default interpolation type.

                Returns:
                    list[float]: The mass flow rates.
            )doc"
        )
        .def(
            "to_tabulated_dynamics",
            &Maneuver::toTabulatedDynamics,
            arg("frame") = Maneuver::DefaultAccelFrameSPtr,
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
            "from_constant_mass_flow_rate_profile",
            &Maneuver::FromConstantMassFlowRateProfile,
            arg("instants"),
            arg("acceleration_profile"),
            arg("frame"),
            arg("mass_flow_rate"),
            R"doc(
                Create a maneuver from a constant mass flow rate profile.

                Args:
                    instants (list[Instant]): An array of instants, must be sorted.
                    acceleration_profile (list[numpy.ndarray]): An acceleration profile of the maneuver, one numpy.ndarray per instant.
                    frame (Frame): A frame in which the acceleration profile is defined.
                    mass_flow_rate (float): The constant mass flow rate.

                Returns:
                    Maneuver: The created maneuver.
            )doc"
        );
}
