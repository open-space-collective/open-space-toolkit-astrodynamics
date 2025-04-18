/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Sequence(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;
    using ostk::core::type::Size;
    using ostk::core::type::String;

    using ostk::physics::time::Duration;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::flight::system::SatelliteSystem;
    using ostk::astrodynamics::trajectory::Segment;
    using ostk::astrodynamics::trajectory::Sequence;
    using ostk::astrodynamics::trajectory::state::NumericalSolver;

    class_<Sequence> sequence(
        aModule,
        "Sequence",
        R"doc(
                A mission `Sequence`. Consists of a list of `Segment` objects and various configuration parameters.

            )doc"
    );

    class_<Sequence::Solution>(
        sequence,
        "Solution",
        R"doc(
            The Solution object that is returned when a `Sequence` is solved.

        )doc"
    )

        .def(
            init<Array<Segment::Solution>, bool>(),
            R"doc(
                Construct a new `Sequence.Solution` object.

                Args:
                segment_solutions (list[Segment.Solution]): The segment solutions.
                execution_is_complete (bool): Whether the execution is complete.

                Returns:
                    Sequence: The new `Sequence.Solution` object.

            )doc",
            arg("segment_solutions"),
            arg("execution_is_complete")
        )

        .def("__str__", &(shiftToString<Sequence::Solution>))
        .def("__repr__", &(shiftToString<Sequence::Solution>))

        .def_readonly(
            "segment_solutions",
            &Sequence::Solution::segmentSolutions,
            R"doc(
                The solutions for each segment.

                :type: list[SegmentSolution]
            )doc"
        )
        .def_readonly(
            "execution_is_complete",
            &Sequence::Solution::executionIsComplete,
            R"doc(
                Whether the execution is complete.

                :type: bool
            )doc"
        )

        .def(
            "access_start_instant",
            &Sequence::Solution::accessStartInstant,
            R"doc(
                Get the instant at which the access starts.

                Returns:
                    Instant: The instant at which the access starts.

            )doc"
        )
        .def(
            "access_end_instant",
            &Sequence::Solution::accessEndInstant,
            R"doc(
                Get the instant at which the access ends.

                Returns:
                    Instant: The instant at which the access ends.

            )doc"
        )
        .def(
            "get_interval",
            &Sequence::Solution::getInterval,
            R"doc(
                Get the interval.

                Returns:
                    Interval: The interval.

            )doc"
        )

        .def(
            "get_states",
            &Sequence::Solution::getStates,
            R"doc(
                Get the states.

                Returns:
                    list[State]: The states.

            )doc"
        )
        .def(
            "get_initial_mass",
            &Sequence::Solution::getInitialMass,
            R"doc(
                Get the initial mass.

                Returns:
                    float: The initial mass.

            )doc"
        )
        .def(
            "get_final_mass",
            &Sequence::Solution::getFinalMass,
            R"doc(
                Get the final mass.

                Returns:
                    float: The final mass.

            )doc"
        )
        .def(
            "get_propagation_duration",
            &Sequence::Solution::getPropagationDuration,
            R"doc(
                Get the propagation duration.

                Returns:
                    Duration: The propagation duration.

            )doc"
        )
        .def(
            "compute_delta_v",
            &Sequence::Solution::computeDeltaV,
            R"doc(
                Compute the delta V.

                Args:
                    specific_impulse (float): The specific impulse.

                Returns:
                    float: The delta V (m/s).

            )doc",
            arg("specific_impulse")
        )
        .def(
            "compute_delta_mass",
            &Sequence::Solution::computeDeltaMass,
            R"doc(
                Compute the delta mass.

                Returns:
                    float: The delta mass.

            )doc"
        )

        .def(
            "calculate_states_at",
            &Sequence::Solution::calculateStatesAt,
            R"doc(
                Calculate states in this sequence's solution at provided instants.

                Args:
                    instants (list[Instant]): The instants at which the states will be calculated.
                    numerical_solver (NumericalSolver): The numerical solver used to calculate the states.

                Returns:
                    list[State]: The states at the provided instants.
                )doc",
            arg("instants"),
            arg("numerical_solver")
        )

        ;

    {
        sequence

            .def(
                init<
                    const Array<Segment>&,
                    const NumericalSolver&,
                    const Array<Shared<Dynamics>>&,
                    const Duration&,
                    const Duration&,
                    const Size&>(),
                R"doc(
                    Construct a new `Sequence` object.

                    Args:
                    segments (list[Segment], optional): The segments. Defaults to an empty list.
                    numerical_solver (NumericalSolver, optional): The numerical solver. Defaults to the default conditional numerical solver.
                    dynamics (list[Dynamics], optional): The dynamics. Defaults to an empty list.
                    maximum_propagation_duration (Duration, optional): The maximum propagation duration. Defaults to 30 days.
                    minimum_maneuver_duration (Duration, optional): The minimum maneuver duration. Defaults to Undefined. If defined, maneuvers less than this duration will be skipped.
                    verbosity (int, optional): The verbosity level. Defaults to 1.

                    Returns:
                        Sequence: The new `Sequence` object.

                )doc",
                arg_v("segments", Array<Segment>::Empty(), "[]"),
                arg_v(
                    "numerical_solver", NumericalSolver::DefaultConditional(), "NumericalSolver.default_conditional()"
                ),
                arg_v("dynamics", Array<Shared<Dynamics>>::Empty(), "[]"),
                arg_v("maximum_propagation_duration", Duration::Days(30.0), "Duration.days(30.0)"),
                arg_v("minimum_maneuver_duration", Duration::Undefined(), "Duration.undefined()"),
                arg("verbosity") = 1
            )

            .def("__str__", &(shiftToString<Sequence>))
            .def("__repr__", &(shiftToString<Sequence>))

            .def(
                "get_segments",
                &Sequence::getSegments,
                R"doc(
                    Get the segments.

                    Returns:
                        list[Segment]: The segments.

                )doc"
            )
            .def(
                "get_numerical_solver",
                &Sequence::getNumericalSolver,
                R"doc(
                    Get the numerical solver.

                    Returns:
                        NumericalSolver: The numerical solver.

                )doc"
            )
            .def(
                "get_dynamics",
                &Sequence::getDynamics,
                R"doc(
                    Get the dynamics.

                    Returns:
                        list[Dynamics]: The dynamics.

                )doc"
            )
            .def(
                "get_maximum_propagation_duration",
                &Sequence::getMaximumPropagationDuration,
                R"doc(
                    Get the maximum propagation duration.

                    Returns:
                        Duration: The maximum propagation duration.

                )doc"
            )
            .def(
                "get_minimum_maneuver_duration",
                &Sequence::getMinimumManeuverDuration,
                R"doc(
                    Get the minimum maneuver duration.

                    Returns:
                        Duration: The minimum maneuver duration.

                )doc"
            )

            .def(
                "add_segment",
                &Sequence::addSegment,
                R"doc(
                    Add a segment.

                    Args:
                        segment (Segment): The segment.

                )doc",
                arg("segment")
            )
            .def(
                "add_segments",
                &Sequence::addSegments,
                arg("segments"),
                R"doc(
                    Add segments.

                    Args:
                        segments (list[Segment]): The segments.

                )doc"
            )
            .def(
                "add_coast_segment",
                &Sequence::addCoastSegment,
                R"doc(
                    Add a coast segment.

                    Args:
                        event_condition (EventCondition): The event condition.

                )doc",
                arg("event_condition")
            )
            .def(
                "add_maneuver_segment",
                &Sequence::addManeuverSegment,
                R"doc(
                    Add a maneuver segment.

                    Args:
                        event_condition (EventCondition): The event condition.
                        thruster_dynamics (Thruster): The thruster dynamics.

                )doc",
                arg("event_condition"),
                arg("thruster_dynamics")
            )

            .def(
                "solve",
                &Sequence::solve,
                R"doc(
                    Solve the sequence.

                    Args:
                        state (State): The state.
                        repetition_count (int, optional): The repetition count. Defaults to 1.

                    Returns:
                        SequenceSolution: The sequence solution.

                )doc",
                arg("state"),
                arg("repetition_count") = 1
            )

            .def(
                "solve_to_condition",
                &Sequence::solveToCondition,
                R"doc(
                    Solve the sequence until the event condition is met.

                    In the case that the event condition is not met due to maximum propagation duration limit,
                    it will return the `SequenceSolution` with `executionIsComplete` set to `False`.

                    Args:
                        state (State): The state.
                        event_condition (EventCondition): The event condition.
                        maximum_propagation_duration_limit (Duration, optional): The maximum propagation duration limit for the sequence. Defaults to 30 days.

                    Returns:
                        SequenceSolution: The sequence solution.

                )doc",
                arg("state"),
                arg("event_condition"),
                arg_v("maximum_propagation_duration_limit", Duration::Days(30.0), "Duration.days(30.0)")
            )

            ;
    }
}
