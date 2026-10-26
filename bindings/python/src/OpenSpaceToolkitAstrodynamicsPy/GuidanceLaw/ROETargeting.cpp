/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>

using namespace pybind11;

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Vector6d;

using ostk::physics::time::Duration;

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

            A plan optimizes `horizon_burn_count` burns at constant thrust, each along a constant direction in the
            spacecraft RTN frame and lasting at most `maximum_burn_duration`, by minimizing the mean tolerance-weighted
            ROE error at the samples (halfway through and at the end of each burn spacing after the planning instant)
            plus `fuel_weight` times the sum of the delta-v norms (normalized by the maximum delta-v of a burn), over
            the delta-v and the timing of the burns. Each burn after the first is centered within one orbit after the
            earliest middle allowed by the previous burn, `burn_spacing` after its middle: the middle of a burn sets
            the argument of latitude of the burn arc, which sets how much of each ROE component its delta-v can
            correct (the eccentricity and inclination vectors each have a best argument of latitude).

            The burns thrust in the tangential-normal plane, and also radially when `allow_radial_thrust` is set.
            Without radial thrust, the along-track position (δλ) is only steered through the semi-major axis (δa), and
            a tangential burn changes δa and the eccentricity vector δe by the same magnitude (2 Δv / (n a)):
            correcting δe without leaving a semi-major axis offset takes two opposite tangential burns about half an
            orbit apart.

            The burn middles are optimized by block coordinate descent: in turn, the middle of each burn is searched
            among `burn_start_candidate_count` times spread over its window, then refined, the delta-v of that burn
            being solved exactly for each candidate. After each sweep over the burns, the delta-v of all the burns are
            optimized jointly, the burn arcs being re-linearized about the previous burn durations (sequential convex
            programming).

            Like the Q-Law, the guidance law decides at every evaluation whether burning now is worth it compared to
            burning at other points of the orbit: it compares the cost J of the plan whose first burn starts now, of
            the best plan, whose first burn starts at its best time within one orbit, and of the plan without the
            first burn, and computes the effectivity of burning now: η = (J_none - J_now) / (J_none - J_best). The spacecraft thrusts along the first burn direction of the plan
            starting now when η >= `effectivity_threshold` and that burn lasts at least `minimum_burn_duration`.

            The guidance law has no memory: the thrust only depends on the instant and the state. A burn should not be
            re-evaluated along its course though: it removes the error that made it worth starting, so that the
            effectivity drops below the threshold before the planned burn is complete. Instead, when a burn starts, the
            guidance law commits to it (see `commit_maneuver_at`): a constant thrust along the planned direction in the
            RTN (QSW) frame, for the planned duration. A maneuvering Segment follows the committed burn, and enforces
            the constraints between burns (minimum separation between maneuvers, maximum maneuver duration, maximum
            maneuver count); `burn_spacing` only models them in the predictions.

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
                const Duration&,
                const Duration&,
                const Real&,
                const Size&,
                const Vector6d&,
                const Real&,
                const Duration&,
                const Real&,
                const Real&,
                const Size&,
                const bool&>(),
            R"doc(
                Constructor.

                Args:
                    burn_spacing (Duration): The minimum time between the middles of two consecutive burns assumed by the predictions, typically the minimum separation between maneuvers plus the maximum burn duration, or half an orbit for at most two maneuvers per orbit (two burns half an orbit apart correct the eccentricity vector with tangential burns without changing the semi-major axis). Each burn is centered within one orbit after the earliest middle allowed by the previous burn.
                    maximum_burn_duration (Duration): The maximum duration of a burn.
                    thrust_acceleration (float): The thrust acceleration used for planning, in m/s^2.
                    horizon_burn_count (int, optional): The number of burns optimized by each plan. Defaults to 5.
                    tolerance_vector (np.ndarray, optional): The tolerance on each ROE component [δa, δλ, δex, δey, δix, δiy], scaled by the reference semi-major axis, in m. Defaults to 10 m on every component.
                    fuel_weight (float, optional): The weight of the delta-v term of the cost, relative to the tracking error term. Defaults to 1.
                    minimum_burn_duration (Duration, optional): Planned burns shorter than this duration are not executed. Defaults to 1 s.
                    relative_semi_major_axis_rate (float, optional): The rate of change of the semi-major axis of the spacecraft relative to the reference, not modeled by J2 (typically differential drag), in m/s. Defaults to 0.
                    effectivity_threshold (float, optional): The effectivity of burning now above which the spacecraft burns, between 0 and 1. Defaults to 0.9.
                    burn_start_candidate_count (int, optional): The number of times, evenly spread over one orbit, among which the timing of each burn is searched before being refined. Defaults to 36.
                    allow_radial_thrust (bool, optional): If True, the burns also thrust radially; otherwise, they only thrust in the tangential-normal plane of the RTN frame. Defaults to False.

                Raises:
                    RuntimeError: If a parameter is undefined or out of range.
            )doc",
            arg("burn_spacing"),
            arg("maximum_burn_duration"),
            arg("thrust_acceleration"),
            arg("horizon_burn_count") = 5,
            arg_v("tolerance_vector", Vector6d::Constant(10.0), "np.full(6, 10.0)"),
            arg("fuel_weight") = 1.0,
            arg_v("minimum_burn_duration", Duration::Seconds(1.0), "Duration.seconds(1.0)"),
            arg("relative_semi_major_axis_rate") = 0.0,
            arg("effectivity_threshold") = 0.9,
            arg("burn_start_candidate_count") = 36,
            arg("allow_radial_thrust") = false
        )
        .def_readonly(
            "burn_spacing",
            &ROETargeting::Parameters::burnSpacing,
            R"doc(
                The minimum time between the middles of two consecutive burns assumed by the predictions.

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
            "horizon_burn_count",
            &ROETargeting::Parameters::horizonBurnCount,
            R"doc(
                The number of burns optimized by each plan.

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
        )
        .def_readonly(
            "effectivity_threshold",
            &ROETargeting::Parameters::effectivityThreshold,
            R"doc(
                The effectivity of burning now above which the spacecraft burns.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "burn_start_candidate_count",
            &ROETargeting::Parameters::burnStartCandidateCount,
            R"doc(
                The number of times, evenly spread over one orbit, among which the timing of each burn is searched.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "allow_radial_thrust",
            &ROETargeting::Parameters::allowRadialThrust,
            R"doc(
                Whether the burns also thrust radially.

                Type:
                    bool
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
                The interval of each burn of the horizon. Burns that are not executed have a zero duration.

                Type:
                    list[Interval]
            )doc"
        )
        .def_readonly(
            "delta_velocities",
            &ROETargeting::Plan::deltaVelocities,
            R"doc(
                The delta-v of each burn of the horizon, in the RTN frame of the spacecraft (with a zero radial
                component unless radial thrust is allowed), in m/s.

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
            "cost",
            &ROETargeting::Plan::cost,
            R"doc(
                The cost of the plan (tracking error term plus delta-v term).

                Type:
                    float
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
            "calculate_thrust_acceleration_at",
            &ROETargeting::calculateThrustAccelerationAt,
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame"),
            R"doc(
                Compute the thrust acceleration.

                Along the first burn direction of the plan starting now, if burning now is effective enough and that
                burn lasts at least the minimum burn duration; zero otherwise.

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
            arg_v("first_burn_delay", Duration::Zero(), "Duration.zero()"),
            R"doc(
                Compute the burn plan from a state.

                The following burns are centered at their best arguments of latitude, each within one orbit after the
                earliest middle allowed by the previous burn.

                Args:
                    state (State): A state, in an inertial frame.
                    first_burn_delay (Duration, optional): The delay of the first burn with respect to the state instant. Defaults to zero.

                Returns:
                    ROETargeting.Plan: The plan.
            )doc"
        )
        .def(
            "compute_best_plan",
            &ROETargeting::computeBestPlan,
            arg("state"),
            R"doc(
                Compute the best burn plan from a state, whose first burn starts at its best time within one orbit.

                The following burns are centered at their best arguments of latitude, each within one orbit after the
                earliest middle allowed by the previous burn.

                Args:
                    state (State): A state, in an inertial frame.

                Returns:
                    ROETargeting.Plan: The plan.
            )doc"
        )
        .def(
            "compute_effectivity",
            &ROETargeting::computeEffectivity,
            arg("state"),
            R"doc(
                Compute the effectivity of burning now.

                Args:
                    state (State): A state, in an inertial frame.

                Returns:
                    float: The effectivity, between 0 and 1 (0 if no burn of the next orbit improves on not burning).
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
