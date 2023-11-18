/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Sequence(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::String;
    using ostk::core::types::Shared;
    using ostk::core::types::Size;

    using ostk::physics::time::Duration;

    using ostk::astro::trajectory::state::NumericalSolver;
    using ostk::astro::trajectory::Sequence;
    using ostk::astro::trajectory::Segment;
    using ostk::astro::Dynamics;
    using ostk::astro::flight::system::SatelliteSystem;

    class_<Sequence::Solution>(
        aModule,
        "SequenceSolution",
        R"doc(
            The Solution object that is returned when a `Sequence` is solved.

            Group:
                trajectory
        )doc"
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
            "compute_delta_mass",
            &Sequence::Solution::computeDeltaMass,
            R"doc(
                Compute the delta mass.

                Returns:
                    float: The delta mass.

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
                    float: The delta V.

            )doc",
            arg("specific_impulse")
        )

        ;

    {
        class_<Sequence>(
            aModule,
            "Sequence",
            R"doc(
                A mission `Sequence`. Consists of a list of `Segment` objects and various configuration parameters.

                Group:
                    trajectory
            )doc"
        )

            .def(
                init<
                    const Array<Segment>&,
                    const Size&,
                    const NumericalSolver&,
                    const Array<Shared<Dynamics>>&,
                    const Duration&,
                    const Size&>(),
                R"doc(
                    Construct a new `Sequence` object.

                    Args:
                    segments (list[Segment], optional): The segments.
                    repetition_count (int, optional): The repetition count.
                    numerical_solver (NumericalSolver, optional): The numerical solver.
                    dynamics (list[Dynamics], optional): The dynamics.
                    maximum_propagation_duration (Duration, optional): The maximum propagation duration.
                    verbosity (int, optional): The verbosity level.

                    Returns:
                        Sequence: The new `Sequence` object.

                )doc",
                arg("segments") = Array<Segment>::Empty(),
                arg("repetition_count") = 1,
                arg("numerical_solver") = NumericalSolver::DefaultConditional(),
                arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
                arg("maximum_propagation_duration") = Duration::Days(30.0),
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

                    This method will terminate early if the maximum propagation duration is exceeded.
                    If an event condition is provided, it will terminate if the event condition is met.
                    In the case that the event condition is not met, it will return the `SequenceSolution` with `executionIsComplete` set to `False`.

                    Args:
                        state (State): The state.
                        maximum_propagation_duration (Duration, optional): The maximum propagation duration.
                        event_condition (EventCondition, optional): The event condition.

                    Returns:
                        SequenceSolution: The sequence solution.

                )doc",
                arg("state"),
                arg("maximum_propagation_duration") = Duration::Days(30.0),
                arg("event_condition") = nullptr
            )

            ;
    }
}
