/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>

using namespace pybind11;

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Vector6d;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::ROETargeting;
using ostk::astrodynamics::Trajectory;

void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_ROETargeting(pybind11::module& aModule)
{
    class_<ROETargeting, GuidanceLaw, Shared<ROETargeting>> roeTargeting(
        aModule,
        "ROETargeting",
        R"doc(
            Full six-element orbit targeting with Relative Orbital Elements (ROE) model predictive control.

            The spacecraft is steered towards a reference trajectory (optionally offset by a constant ROE vector),
            including its along-track position (phasing), which Q-Law, a five-element law, cannot target.

            The relative state is the quasi-nonsingular ROE [δa, δλ, δex, δey, δix, δiy] with respect to the
            reference, computed from Brouwer-Lyddane mean elements. Its free motion is predicted with the J2 state
            transition matrix of Koenig, Guffanti and D'Amico (2017), augmented with an optional constant relative
            semi-major axis rate (typically differential drag). Burns enter the model through the near-circular Gauss
            variational equations, averaged over the burn arc.

            Burns may only happen in periodic windows of duration `maximum_burn_duration`, starting every
            `burn_window_period` from `burn_window_epoch`. A window holds at most one burn, starting at the window
            start, at constant thrust, along a constant direction in the spacecraft RTN frame.

            At the start of every window, the burns of the next `horizon_window_count` windows are optimized by
            minimizing the mean tolerance-weighted ROE error at the samples (halfway through and at the end of each
            window period) plus `fuel_weight` times the sum of the delta-v norms (normalized by the maximum delta-v of
            a burn), subject to the maximum burn duration. The burn arcs are re-linearized about the previous burn
            durations (sequential convex programming). Only the first burn is executed; the next window re-plans
            from the then-current state.

            The plan of a window is computed at the first evaluation within that window and held until its end. A new
            propagation starting inside a window that was already planned reuses the held plan: use a new instance in
            that case. Start each burn window propagation exactly at the window start for deterministic results.

            The model assumes a near-circular reference orbit around the Earth (EGM2008 constants).

            - Ref: Koenig, Guffanti, D'Amico, New State Transition Matrices for Spacecraft Relative Motion in Perturbed Orbits, JGCD 2017.
            - Ref: D'Amico, Autonomous Formation Flying in Low Earth Orbit, 2010.
        )doc"
    );

    class_<ROETargeting::Parameters>(
        roeTargeting,
        "Parameters",
        R"doc(
            ROE targeting parameters.
        )doc"
    )
        .def(
            init<
                const Instant&,
                const Duration&,
                const Duration&,
                const Real&,
                const Size&,
                const Vector6d&,
                const Real&,
                const Duration&,
                const Real&>(),
            R"doc(
                Constructor.

                Args:
                    burn_window_epoch (Instant): The start of the first burn window.
                    burn_window_period (Duration): The time between the starts of two consecutive burn windows.
                    maximum_burn_duration (Duration): The maximum duration of a burn, which is also the duration of a burn window.
                    thrust_acceleration (float): The thrust acceleration used for planning, in m/s^2.
                    horizon_window_count (int, optional): The number of burn windows optimized at each planning. Defaults to 5.
                    tolerance_vector (np.ndarray, optional): The tolerance on each ROE component [δa, δλ, δex, δey, δix, δiy], scaled by the reference semi-major axis, in m. Defaults to 10 m on every component.
                    fuel_weight (float, optional): The weight of the delta-v term of the cost, relative to the tracking error term. Defaults to 1.
                    minimum_burn_duration (Duration, optional): Planned burns shorter than this duration are not executed. Defaults to 1 s.
                    relative_semi_major_axis_rate (float, optional): The rate of change of the semi-major axis of the spacecraft relative to the reference, not modeled by J2 (typically differential drag), in m/s. Defaults to 0.

                Raises:
                    RuntimeError: If a parameter is undefined or out of range.
            )doc",
            arg("burn_window_epoch"),
            arg("burn_window_period"),
            arg("maximum_burn_duration"),
            arg("thrust_acceleration"),
            arg("horizon_window_count") = 5,
            arg_v("tolerance_vector", Vector6d::Constant(10.0), "np.full(6, 10.0)"),
            arg("fuel_weight") = 1.0,
            arg_v("minimum_burn_duration", Duration::Seconds(1.0), "Duration.seconds(1.0)"),
            arg("relative_semi_major_axis_rate") = 0.0
        )
        .def_readonly(
            "burn_window_epoch",
            &ROETargeting::Parameters::burnWindowEpoch,
            R"doc(
                The start of the first burn window.

                Type:
                    Instant
            )doc"
        )
        .def_readonly(
            "burn_window_period",
            &ROETargeting::Parameters::burnWindowPeriod,
            R"doc(
                The time between the starts of two consecutive burn windows.

                Type:
                    Duration
            )doc"
        )
        .def_readonly(
            "maximum_burn_duration",
            &ROETargeting::Parameters::maximumBurnDuration,
            R"doc(
                The maximum duration of a burn.

                Type:
                    Duration
            )doc"
        )
        .def_readonly(
            "thrust_acceleration",
            &ROETargeting::Parameters::thrustAcceleration,
            R"doc(
                The thrust acceleration used for planning, in m/s^2.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "horizon_window_count",
            &ROETargeting::Parameters::horizonWindowCount,
            R"doc(
                The number of burn windows optimized at each planning.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "tolerance_vector",
            &ROETargeting::Parameters::toleranceVector,
            R"doc(
                The tolerance on each ROE component, in m.

                Type:
                    np.ndarray
            )doc"
        )
        .def_readonly(
            "fuel_weight",
            &ROETargeting::Parameters::fuelWeight,
            R"doc(
                The weight of the delta-v term of the cost.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "minimum_burn_duration",
            &ROETargeting::Parameters::minimumBurnDuration,
            R"doc(
                Planned burns shorter than this duration are not executed.

                Type:
                    Duration
            )doc"
        )
        .def_readonly(
            "relative_semi_major_axis_rate",
            &ROETargeting::Parameters::relativeSemiMajorAxisRate,
            R"doc(
                The relative semi-major axis rate not modeled by J2, in m/s.

                Type:
                    float
            )doc"
        );

    class_<ROETargeting::Plan>(
        roeTargeting,
        "Plan",
        R"doc(
            A burn plan over the planning horizon.
        )doc"
    )
        .def_readonly(
            "epoch",
            &ROETargeting::Plan::epoch,
            R"doc(
                The planning epoch.

                Type:
                    Instant
            )doc"
        )
        .def_readonly(
            "reference_semi_major_axis",
            &ROETargeting::Plan::referenceSemiMajorAxis,
            R"doc(
                The reference semi-major axis at the planning epoch, in m.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "relative_orbital_elements",
            &ROETargeting::Plan::relativeOrbitalElements,
            R"doc(
                The ROE with respect to the reference at the planning epoch, scaled by the reference semi-major axis, in m.

                Type:
                    np.ndarray
            )doc"
        )
        .def_readonly(
            "burn_intervals",
            &ROETargeting::Plan::burnIntervals,
            R"doc(
                The burn interval of each window of the horizon. Windows without a burn have a zero duration.

                Type:
                    list[Interval]
            )doc"
        )
        .def_readonly(
            "delta_velocities",
            &ROETargeting::Plan::deltaVelocities,
            R"doc(
                The delta-v of each window of the horizon, in the RTN frame of the spacecraft, in m/s.

                Type:
                    list[np.ndarray]
            )doc"
        )
        .def_readonly(
            "prediction_instants",
            &ROETargeting::Plan::predictionInstants,
            R"doc(
                The instants at which the ROE are predicted.

                Type:
                    list[Instant]
            )doc"
        )
        .def_readonly(
            "predicted_relative_orbital_elements",
            &ROETargeting::Plan::predictedRelativeOrbitalElements,
            R"doc(
                The predicted ROE with respect to the reference, scaled by the reference semi-major axis, in m.

                Type:
                    list[np.ndarray]
            )doc"
        )
        .def_readonly(
            "iteration_count",
            &ROETargeting::Plan::iterationCount,
            R"doc(
                The number of sequential convex programming iterations.

                Type:
                    int
            )doc"
        );

    roeTargeting

        .def(
            init<const Trajectory&, const ROETargeting::Parameters&, const Vector6d&>(),
            R"doc(
                Constructor.

                Args:
                    reference_trajectory (Trajectory): The reference trajectory. Must provide states in an inertial frame.
                    parameters (ROETargeting.Parameters): The parameters.
                    target_relative_orbital_elements (np.ndarray, optional): The ROE to hold with respect to the reference, scaled by the reference semi-major axis, in m. Defaults to zero.
            )doc",
            arg("reference_trajectory"),
            arg("parameters"),
            arg_v("target_relative_orbital_elements", Vector6d::Zero(), "np.zeros(6)")
        )

        .def("__str__", &(shiftToString<ROETargeting>))
        .def("__repr__", &(shiftToString<ROETargeting>))

        .def(
            "access_reference_trajectory",
            &ROETargeting::accessReferenceTrajectory,
            return_value_policy::reference_internal,
            R"doc(
                Access the reference trajectory.

                Returns:
                    Trajectory: The reference trajectory.
            )doc"
        )
        .def(
            "get_parameters",
            &ROETargeting::getParameters,
            R"doc(
                Get the parameters.

                Returns:
                    ROETargeting.Parameters: The parameters.
            )doc"
        )
        .def(
            "get_target_relative_orbital_elements",
            &ROETargeting::getTargetRelativeOrbitalElements,
            R"doc(
                Get the target ROE.

                Returns:
                    np.ndarray: The target ROE, scaled by the reference semi-major axis, in m.
            )doc"
        )
        .def(
            "get_burn_window_at",
            &ROETargeting::getBurnWindowAt,
            arg("instant"),
            R"doc(
                Get the burn window containing an instant, or the next one if the instant is between windows.

                Args:
                    instant (Instant): An instant.

                Returns:
                    Interval: The burn window.
            )doc"
        )
        .def(
            "calculate_thrust_acceleration_at",
            &ROETargeting::calculateThrustAccelerationAt,
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame"),
            R"doc(
                Compute the thrust acceleration.

                Within a burn window, thrust along the planned direction until the planned burn duration has elapsed;
                zero otherwise.

                Args:
                    instant (Instant): An instant.
                    position_coordinates (np.ndarray): The position coordinates, in an inertial frame.
                    velocity_coordinates (np.ndarray): The velocity coordinates, in an inertial frame.
                    thrust_acceleration (float): The thrust acceleration.
                    output_frame (Frame): The frame of the coordinates and of the returned acceleration.

                Returns:
                    np.ndarray: The thrust acceleration.
            )doc"
        )
        .def(
            "compute_relative_orbital_elements",
            &ROETargeting::computeRelativeOrbitalElements,
            arg("state"),
            R"doc(
                Compute the ROE of a state with respect to the reference.

                Args:
                    state (State): A state, in an inertial frame.

                Returns:
                    np.ndarray: The ROE [δa, δλ, δex, δey, δix, δiy], scaled by the reference semi-major axis, in m.
            )doc"
        )
        .def(
            "compute_plan",
            &ROETargeting::computePlan,
            arg("state"),
            R"doc(
                Compute the burn plan from a state.

                The planning horizon starts with the first burn window starting at or after the state instant.

                Args:
                    state (State): A state, in an inertial frame.

                Returns:
                    ROETargeting.Plan: The plan.
            )doc"
        )

        .def_static(
            "compute_relative_orbital_elements_from_coe",
            &ROETargeting::ComputeRelativeOrbitalElements,
            arg("coe"),
            arg("reference_coe"),
            R"doc(
                Compute the quasi-nonsingular ROE between two sets of mean orbital elements.

                Args:
                    coe (COE): The mean orbital elements of the spacecraft.
                    reference_coe (COE): The mean orbital elements of the reference.

                Returns:
                    np.ndarray: The dimensionless ROE [δa, δλ, δex, δey, δix, δiy].
            )doc"
        )
        .def_static(
            "compute_state_transition_matrix",
            &ROETargeting::ComputeStateTransitionMatrix,
            arg("reference_coe"),
            arg("duration"),
            R"doc(
                Compute the J2 state transition matrix of the quasi-nonsingular ROE (Koenig, Guffanti, D'Amico).

                Args:
                    reference_coe (COE): The mean orbital elements of the reference at the start of the transition.
                    duration (Duration): The transition duration.

                Returns:
                    np.ndarray: The 6x6 state transition matrix.
            )doc"
        )
        .def_static(
            "compute_control_input_matrix",
            &ROETargeting::ComputeControlInputMatrix,
            arg("semi_major_axis"),
            arg("mean_argument_of_latitude_start"),
            arg("mean_argument_of_latitude_end"),
            R"doc(
                Compute the control input matrix, mapping a delta-v in the RTN frame (m/s) to the dimensionless ROE,
                from the near-circular Gauss variational equations averaged over an arc of mean argument of latitude.

                Args:
                    semi_major_axis (float): The reference semi-major axis, in m.
                    mean_argument_of_latitude_start (float): The mean argument of latitude at the start of the arc, in rad.
                    mean_argument_of_latitude_end (float): The mean argument of latitude at the end of the arc, in rad.

                Returns:
                    np.ndarray: The 6x3 control input matrix.
            )doc"
        )

        ;
}
