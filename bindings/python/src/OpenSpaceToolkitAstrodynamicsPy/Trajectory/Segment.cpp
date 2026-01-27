/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::flight::Maneuver;
    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
    using ostk::astrodynamics::trajectory::Segment;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;
    using ostk::astrodynamics::trajectory::state::NumericalSolver;

    class_<Segment> segment(
        aModule,
        "Segment",
        R"doc(
                A `Segment` that can be solved provided an initial `State` and termination `Event Condition`.

            )doc"
    );

    enum_<Segment::Type>(
        segment,
        "Type",
        R"doc(
            Segment type.
        )doc"
    )

        .value("Coast", Segment::Type::Coast, "Coast")
        .value("Maneuver", Segment::Type::Maneuver, "Maneuver")

        ;

    enum_<Segment::MaximumManeuverDurationViolationStrategy>(
        segment,
        "MaximumManeuverDurationViolationStrategy",
        R"doc(
            Strategy to use when a maneuver exceeds the maximum duration constraint.

            Fail: Will throw a RuntimeError if a maneuver exceeds the maximum duration.
            Skip: The maneuver will be skipped entirely.
            TruncateEnd: The maneuver will be shortened to the maximum duration, truncating the trailing edge.
            TruncateStart: The maneuver will be shortened to the maximum duration, truncating the leading edge.
            Center: The maneuver will be shortened to the maximum duration, truncating the edges, keeping the centered part of the maneuver.
            Chunk: The maneuver will be split into chunks from the leading edge.

            Example:
            Maximum duration:  [------]
            Proposed maneuver: [---------------------------------]
            TruncateEnd:       [------]
            Center:                          [------]
            TruncateStart:                                [------]
            Chunk:             [------]  [------]  [------]  [---]
        )doc"
    )

        .value(
            "Fail",
            Segment::MaximumManeuverDurationViolationStrategy::Fail,
            "Will throw an exception if a maneuver exceeds the maximum duration."
        )
        .value(
            "Skip", Segment::MaximumManeuverDurationViolationStrategy::Skip, "The maneuver will be skipped entirely."
        )
        .value(
            "TruncateEnd",
            Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd,
            "The maneuver will be shortened to the maximum duration, truncating the trailing edge."
        )
        .value(
            "TruncateStart",
            Segment::MaximumManeuverDurationViolationStrategy::TruncateStart,
            "The maneuver will be shortened to the maximum duration, truncating the leading edge."
        )
        .value(
            "Center",
            Segment::MaximumManeuverDurationViolationStrategy::Center,
            "The maneuver will be shortened to the maximum duration, truncating the edges, keeping the centered part "
            "of the maneuver."
        )
        .value(
            "Chunk",
            Segment::MaximumManeuverDurationViolationStrategy::Chunk,
            "The maneuver will be split into chunks from the leading edge."
        )

        ;

    class_<Segment::Solution>(
        segment,
        "Solution",
        R"doc(
            The Solution object returned when a `Segment` is solved.

        )doc"
    )

        .def(
            init<const String&, const Array<Shared<Dynamics>>&, const Array<State>&, const bool&, const Segment::Type&>(
            ),
            arg("name"),
            arg("dynamics"),
            arg("states"),
            arg("condition_is_satisfied"),
            arg("segment_type"),
            R"doc(
                Construct a Segment Solution.

                Args:
                    name (str): The name of the segment.
                    dynamics (list[Dynamics]): The dynamics.
                    states (list[State]): The states.
                    condition_is_satisfied (bool): Whether the event condition is satisfied.
                    segment_type (Type): The type of the segment.

            )doc"
        )
        .def(
            init<
                const String&,
                const Array<Shared<Dynamics>>&,
                const Array<State>&,
                const bool&,
                const Segment::Type&,
                const Array<Interval>&>(),
            arg("name"),
            arg("dynamics"),
            arg("states"),
            arg("condition_is_satisfied"),
            arg("segment_type"),
            arg("maneuver_intervals"),
            R"doc(
                Construct a Segment Solution with maneuver intervals.

                Args:
                    name (str): The name of the segment.
                    dynamics (list[Dynamics]): The dynamics.
                    states (list[State]): The states.
                    condition_is_satisfied (bool): Whether the event condition is satisfied.
                    segment_type (Type): The type of the segment.
                    maneuver_intervals (list[Interval]): The maneuver intervals (for maneuver segments).

            )doc"
        )

        .def("__str__", &(shiftToString<Segment::Solution>))
        .def("__repr__", &(shiftToString<Segment::Solution>))

        .def_readonly(
            "name",
            &Segment::Solution::name,
            R"doc(
                The name of the segment.

                :type: str
            )doc"
        )
        .def_readonly(
            "dynamics",
            &Segment::Solution::dynamics,
            R"doc(
                The dynamics.

                :type: Dynamics
            )doc"
        )
        .def_readonly(
            "states",
            &Segment::Solution::states,
            R"doc(
                The states.

                :type: list[State]
            )doc"
        )
        .def_readonly(
            "condition_is_satisfied",
            &Segment::Solution::conditionIsSatisfied,
            R"doc(
                Whether the event condition is satisfied.

                :type: bool
            )doc"
        )
        .def_readonly(
            "segment_type",
            &Segment::Solution::segmentType,
            R"doc(
                The type of the segment.

                :type: Type
            )doc"
        )
        .def_readonly(
            "maneuver_intervals",
            &Segment::Solution::maneuverIntervals,
            R"doc(
                The maneuver intervals (for maneuver segments).

                :type: list[Interval]
            )doc"
        )

        .def(
            "access_start_instant",
            &Segment::Solution::accessStartInstant,
            R"doc(
                Get the instant at which the segment starts.

                Returns:
                    Instant: The instant at which the segment starts.

            )doc"
        )
        .def(
            "access_end_instant",
            &Segment::Solution::accessEndInstant,
            R"doc(
                Get the instant at which the segment ends.

                Returns:
                    Instant: The instant at which the segment ends.

            )doc"
        )

        .def(
            "get_interval",
            &Segment::Solution::getInterval,
            R"doc(
                Get the time interval of the solution.

                Returns:
                    Interval: The interval.

            )doc"
        )
        .def(
            "get_initial_mass",
            &Segment::Solution::getInitialMass,
            R"doc(
                Get the initial mass.

                Returns:
                    Mass: The initial mass.

            )doc"
        )
        .def(
            "get_final_mass",
            &Segment::Solution::getFinalMass,
            R"doc(
                Get the final mass.

                Returns:
                    Mass: The final mass.

            )doc"
        )
        .def(
            "get_propagation_duration",
            &Segment::Solution::getPropagationDuration,
            R"doc(
                Get the propagation duration.

                Returns:
                    Duration: The propagation duration.

            )doc"
        )
        .def(
            "get_thruster_dynamics",
            &Segment::Solution::getThrusterDynamics,
            R"doc(
                Get the thruster dynamics from the solution.

                Returns:
                    Thruster: The thruster dynamics.

            )doc"
        )

        .def(
            "compute_delta_v",
            &Segment::Solution::computeDeltaV,
            arg("specific_impulse"),
            R"doc(
                Compute the delta V.

                Args:
                    specific_impulse (float): The specific impulse.

                Returns:
                    float: The delta V (m/s).

            )doc"
        )
        .def(
            "compute_delta_mass",
            &Segment::Solution::computeDeltaMass,
            R"doc(
                Compute the delta mass.

                Returns:
                    Mass: The delta mass.

            )doc"
        )

        .def(
            "extract_maneuvers",
            &Segment::Solution::extractManeuvers,
            arg("frame"),
            R"doc(
            Extract maneuvers from the (maneuvering) segment.

            Returns:
                list[Maneuver]: The list of maneuvers.

            )doc"
        )

        .def(
            "calculate_states_at",
            &Segment::Solution::calculateStatesAt,
            arg("instants"),
            arg("numerical_solver"),
            R"doc(
                Calculate the states in this segment's solution at the given instants.

                Args:
                    instants (list[Instant]): The instants at which the states will be calculated.
                    numerical_solver (NumericalSolver): The numerical solver used to calculate the states.

                Returns:
                    list[State]: The states at the provided instants.
            )doc"
        )
        .def(
            "get_dynamics_contribution",
            &Segment::Solution::getDynamicsContribution,
            arg("dynamics"),
            arg("frame"),
            arg_v("coordinate_subsets", Array<Shared<const CoordinateSubset>>::Empty(), "[]"),
            R"doc(
                Compute the contribution of the provided dynamics in the provided frame for all states associated with the segment.

                Args:
                    dynamics (Dynamics): The dynamics.
                    frame (Frame): The frame.
                    coordinate_subsets (list[CoordinateSubset], optional): A subset of the dynamics writing coordinate subsets to consider.

                Returns:
                    MatrixXd: The matrix of dynamics contributions for the selected coordinate subsets of the dynamics.

            )doc"
        )
        .def(
            "get_dynamics_acceleration_contribution",
            &Segment::Solution::getDynamicsAccelerationContribution,
            arg("dynamics"),
            arg("frame"),
            R"doc(
                Compute the contribution of the provided dynamics to the acceleration in the provided frame for all states associated with the segment.

                Args:
                    dynamics (Dynamics): The dynamics.
                    frame (Frame): The frame.

                Returns:
                    np.ndarray: The matrix of dynamics contributions to acceleration.

            )doc"
        )
        .def(
            "get_all_dynamics_contributions",
            &Segment::Solution::getAllDynamicsContributions,
            arg("frame"),
            R"doc(
                Compute the contributions of all segment's dynamics in the provided frame for all states assocated with the segment.

                Args:
                    frame (Frame): The frame.

                Returns:
                    dict[Dynamics, np.ndarray]: The list of matrices with individual dynamics contributions.

            )doc"
        )

        ;

    class_<Segment::ManeuverConstraints>(
        segment,
        "ManeuverConstraints",
        R"doc(
            Constraints for maneuver segments.
        )doc"
    )

        .def(
            init<>(),
            R"doc(
                Default constructor. All durations are undefined and strategy is Fail.
            )doc"
        )

        .def(
            init<
                const Duration&,
                const Duration&,
                const Duration&,
                const Segment::MaximumManeuverDurationViolationStrategy&>(),
            arg_v("minimum_duration", Duration::Undefined(), "Duration.undefined()"),
            arg_v("maximum_duration", Duration::Undefined(), "Duration.undefined()"),
            arg_v("minimum_separation", Duration::Undefined(), "Duration.undefined()"),
            arg_v(
                "maximum_duration_strategy",
                Segment::MaximumManeuverDurationViolationStrategy::Fail,
                "Segment.MaximumManeuverDurationViolationStrategy.Fail"
            ),
            R"doc(
                Construct ManeuverConstraints with specific parameters.

                Args:
                    minimum_duration (Duration, optional): The minimum duration for a maneuver. Defaults to Duration.undefined().
                    maximum_duration (Duration, optional): The maximum duration for a maneuver. Defaults to Duration.undefined().
                    minimum_separation (Duration, optional): The minimum separation between maneuvers. Defaults to Duration.undefined().
                    maximum_duration_strategy (MaximumManeuverDurationViolationStrategy, optional): The strategy when maximum duration is violated. Defaults to Segment.MaximumManeuverDurationViolationStrategy.Fail.
            )doc"
        )

        .def("__str__", &(shiftToString<Segment::ManeuverConstraints>))
        .def("__repr__", &(shiftToString<Segment::ManeuverConstraints>))

        .def_readwrite(
            "minimum_duration",
            &Segment::ManeuverConstraints::minimumDuration,
            R"doc(
                The minimum duration for a maneuver.

                :type: Duration
            )doc"
        )
        .def_readwrite(
            "maximum_duration",
            &Segment::ManeuverConstraints::maximumDuration,
            R"doc(
                The maximum duration for a maneuver.

                :type: Duration
            )doc"
        )
        .def_readwrite(
            "minimum_separation",
            &Segment::ManeuverConstraints::minimumSeparation,
            R"doc(
                The minimum separation between maneuvers.

                :type: Duration
            )doc"
        )
        .def_readwrite(
            "maximum_duration_strategy",
            &Segment::ManeuverConstraints::maximumDurationStrategy,
            R"doc(
                The strategy to use when a maneuver exceeds the maximum duration.

                :type: MaximumManeuverDurationViolationStrategy
            )doc"
        )

        .def(
            "is_defined",
            &Segment::ManeuverConstraints::isDefined,
            R"doc(
                Check if any of the constraints are defined.

                Returns:
                    bool: True if at least one constraint is defined, False otherwise.
            )doc"
        )

        .def(
            "interval_has_valid_minimum_duration",
            &Segment::ManeuverConstraints::intervalHasValidMinimumDuration,
            arg("interval"),
            R"doc(
                Check if the interval has a valid minimum duration.
            )doc"
        )
        .def(
            "interval_has_valid_maximum_duration",
            &Segment::ManeuverConstraints::intervalHasValidMaximumDuration,
            arg("interval"),
            R"doc(
                Check if the interval has a valid maximum duration.
            )doc"
        )

        ;

    segment

        .def("__str__", &(shiftToString<Segment>))
        .def("__repr__", &(shiftToString<Segment>))

        .def(
            "get_name",
            &Segment::getName,
            R"doc(
                Get the name of the segment.

                Returns:
                    str: The name of the segment.

            )doc"
        )
        .def(
            "get_event_condition",
            &Segment::getEventCondition,
            R"doc(
                Get the event condition.

                Returns:
                    EventCondition: The event condition.

            )doc"
        )
        .def(
            "get_dynamics",
            &Segment::getDynamics,
            R"doc(
                Get the dynamics.

                Returns:
                    Dynamics: The dynamics.

            )doc"
        )
        .def(
            "get_free_dynamics",
            &Segment::getFreeDynamics,
            R"doc(
                Get the free dynamics array, devoid of any thruster dynamics.

                Returns:
                    list[Dynamics]: The free dynamics array.

            )doc"
        )
        .def(
            "get_numerical_solver",
            &Segment::getNumericalSolver,
            R"doc(
                Get the numerical solver.

                Returns:
                    NumericalSolver: The numerical solver.

            )doc"
        )
        .def(
            "get_thruster_dynamics",
            &Segment::getThrusterDynamics,
            R"doc(
                Get the thruster dynamics.

                Returns:
                    Thruster: The thruster dynamics.

            )doc"
        )
        .def(
            "get_maneuver_constraints",
            &Segment::getManeuverConstraints,
            R"doc(
                Get the maneuver constraints.

                Returns:
                    ManeuverConstraints: The maneuver constraints.

            )doc"
        )
        .def(
            "get_type",
            &Segment::getType,
            R"doc(
                Get the type of the segment.

                Returns:
                    Type: The type of the segment.

            )doc"
        )

        .def(
            "solve",
            &Segment::solve,
            arg("state"),
            arg_v("maximum_propagation_duration", Duration::Days(30.0), "Duration.days(30.0)"),
            arg_v("previous_maneuver_interval", Interval::Undefined(), "Interval.undefined()"),
            R"doc(
                Solve the segment until its event condition is satisfied or the maximum propagation duration is reached.

                Args:
                    state (State): The state.
                    maximum_propagation_duration (Duration, optional): The maximum propagation duration. Defaults to 30 days.
                    previous_maneuver_interval (Interval, optional): The previous maneuver interval prior to this segment. Defaults to Interval.undefined().

                Returns:
                    SegmentSolution: The segment solution.

            )doc"
        )

        .def_static(
            "coast",
            &Segment::Coast,
            arg("name"),
            arg("event_condition"),
            arg("dynamics"),
            arg("numerical_solver"),
            R"doc(
                Create a coast segment.

                Args:
                    name (str): The name of the segment.
                    event_condition (EventCondition): The event condition.
                    dynamics (Dynamics): The dynamics.
                    numerical_solver (NumericalSolver): The numerical solver.

                Returns:
                    Segment: The coast segment.
            )doc"
        )

        .def_static(
            "maneuver",
            &Segment::Maneuver,
            arg("name"),
            arg("event_condition"),
            arg("thruster_dynamics"),
            arg("dynamics"),
            arg("numerical_solver"),
            arg_v("maneuver_constraints", Segment::ManeuverConstraints(), "ManeuverConstraints()"),
            R"doc(
                Create a maneuver segment.

                Args:
                    name (str): The name of the segment.
                    event_condition (EventCondition): The event condition.
                    thruster_dynamics (ThrusterDynamics): The thruster dynamics.
                    dynamics (Dynamics): The dynamics.
                    numerical_solver (NumericalSolver): The numerical solver.
                    maneuver_constraints (ManeuverConstraints, optional): The maneuver constraints. Defaults to empty constraints.

                Returns:
                    Segment: The maneuver segment.
            )doc"
        )

        .def_static(
            "constant_local_orbital_frame_direction_maneuver",
            &Segment::ConstantLocalOrbitalFrameDirectionManeuver,
            arg("name"),
            arg("event_condition"),
            arg("thruster_dynamics"),
            arg("dynamics"),
            arg("numerical_solver"),
            arg("local_orbital_frame_factory"),
            arg_v("maximum_allowed_angular_offset", Angle::Undefined(), "Angle.undefined()"),
            arg_v("maneuver_constraints", Segment::ManeuverConstraints(), "ManeuverConstraints()"),
            R"doc(
                Create a maneuvering segment that produces maneuvers with a constant direction in the local orbital frame.

                The provided thruster dynamics are used to solve the segment at first. The maneuvers produced by this segement solution
                are then used to create a new thruster dynamics with a constant direction in the local orbital frame. This new thruster dynamics
                is then used to actually solve the segment.

                If defined, a runtime error will be thrown if the maximum allowed angular offset between the original thruster dynamics
                and the mean thrust direction is violated.

                Args:
                    name (str): The name of the segment.
                    event_condition (EventCondition): The event condition.
                    thruster_dynamics (ThrusterDynamics): The thruster dynamics.
                    dynamics (Dynamics): The dynamics.
                    numerical_solver (NumericalSolver): The numerical solver.
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): The local orbital frame factory.
                    maximum_allowed_angular_offset (Angle, optional): The maximum allowed angular offset to consider (if any). Defaults to Angle.undefined().
                    maneuver_constraints (ManeuverConstraints, optional): The maneuver constraints. Defaults to empty constraints.

                Returns:
                    Segment: The maneuver segment.
            )doc"
        )

        .def_static(
            "string_from_maximum_maneuver_duration_violation_strategy",
            &Segment::StringFromMaximumManeuverDurationViolationStrategy,
            arg("strategy"),
            R"doc(
                Get the string representation of a MaximumManeuverDurationViolationStrategy.

                Args:
                    strategy (MaximumManeuverDurationViolationStrategy): The strategy.

                Returns:
                    str: The string representation of the strategy.
            )doc"
        )

        ;
}
