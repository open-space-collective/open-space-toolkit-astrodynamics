/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::physics::time::Duration;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::flight::Maneuver;
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
            "get_numerical_solver",
            &Segment::getNumericalSolver,
            R"doc(
                Get the numerical solver.

                Returns:
                    NumericalSolver: The numerical solver.

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
            R"doc(
                Solve the segment.

                Args:
                    state (State): The state.
                    maximum_propagation_duration (Duration, optional): The maximum propagation duration.

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
            R"doc(
                Create a maneuver segment.

                Args:
                    name (str): The name of the segment.
                    event_condition (EventCondition): The event condition.
                    thruster_dynamics (ThrusterDynamics): The thruster dynamics.
                    dynamics (Dynamics): The dynamics.
                    numerical_solver (NumericalSolver): The numerical solver.

                Returns:
                    Segment: The maneuver segment.
            )doc"
        )

        ;
}
