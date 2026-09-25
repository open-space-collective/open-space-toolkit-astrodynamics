/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::physics::Environment;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::EventCondition;
    using ostk::astrodynamics::flight::Maneuver;
    using ostk::astrodynamics::flight::system::SatelliteSystem;
    using ostk::astrodynamics::trajectory::Propagator;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::NumericalSolver;

    class_<Propagator> propagator(
        aModule,
        "Propagator",
        R"doc(
            A `Propagator` that propagates the provided `State` using it's `NumericalSolver` under the set `Dynamics`.

        )doc"
    );

    class_<Propagator::StepContributions>(
        propagator,
        "StepContributions",
        R"doc(
            Per-dynamics contributions recorded at each solver step accepted during a propagation.

        )doc"
    )
        .def_readonly(
            "instants",
            &Propagator::StepContributions::instants,
            R"doc(
                One instant per recorded step, in integration order.

                Type:
                    list[Instant]
            )doc"
        )
        .def_readonly(
            "contributions",
            &Propagator::StepContributions::contributions,
            R"doc(
                Per dynamics, a matrix whose rows align with `instants` and whose columns follow the
                dynamics' write coordinate subsets, expressed in GCRF.

                Type:
                    dict[Dynamics, np.ndarray]
            )doc"
        )

        ;

    propagator

        .def(
            init<const NumericalSolver&, const Array<Shared<Dynamics>>&>(),
            arg("numerical_solver"),
            arg_v("dynamics", Array<Shared<Dynamics>>::Empty(), "[]"),
            R"doc(
                Construct a new `Propagator` object.

                Args:
                    numerical_solver (NumericalSolver) The numerical solver.
                    dynamics (list[Dynamics], optional) The dynamics.

                Returns:
                    Propagator: The new `Propagator` object.

            )doc"
        )

        .def(
            init<
                const NumericalSolver&,
                const Array<Shared<Dynamics>>&,
                const Array<Maneuver>&,
                const Interpolator::Type&>(),
            arg("numerical_solver"),
            arg("dynamics"),
            arg("maneuvers"),
            arg_v(
                "interpolation_type",
                DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE,
                "Interpolator.Type.BarycentricRational"
            ),
            R"doc(
                Construct a new `Propagator` object with maneuvers.

                Args:
                    numerical_solver (NumericalSolver) The numerical solver.
                    dynamics (list[Dynamics]) The dynamics.
                    maneuvers (list[Maneuver]) The maneuvers.
                    interpolation_type (Interpolator.Type, optional) The interpolation type. Defaults to Barycentric Rational.

                Returns:
                    Propagator: The new `Propagator` object.
            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Propagator>))
        .def("__repr__", &(shiftToString<Propagator>))

        .def(
            "is_defined",
            &Propagator::isDefined,
            R"doc(
                Check if the propagator is defined.

                Returns:
                    bool: True if the propagator is defined, False otherwise.

            )doc"
        )

        .def(
            "is_contribution_observation_enabled",
            &Propagator::isContributionObservationEnabled,
            R"doc(
                Check if per-dynamics step contribution recording is enabled.

                Returns:
                    bool: True if per-dynamics step contribution recording is enabled, False otherwise.

            )doc"
        )

        .def(
            "access_numerical_solver",
            &Propagator::accessNumericalSolver,
            R"doc(
                Access the numerical solver.

                Returns:
                    NumericalSolver&: The numerical solver.

            )doc"
        )

        .def(
            "get_number_of_coordinates",
            &Propagator::getNumberOfCoordinates,
            R"doc(
                Get the number of coordinates.

                Returns:
                    int: The number of coordinates.

            )doc"
        )
        .def(
            "get_dynamics",
            &Propagator::getDynamics,
            R"doc(
                Get the dynamics.

                Returns:
                    list[Dynamics]: The dynamics.

            )doc"
        )
        .def(
            "get_recorded_step_contributions",
            &Propagator::getRecordedStepContributions,
            R"doc(
                Get the step contributions recorded during the most recent `calculate_*` call.

                Empty unless recording is enabled with `set_contribution_observation_enabled`. For
                `calculate_state_at` and `calculate_state_to_condition`, the recorded steps are the solver's
                accepted steps (initial and final states included). For `calculate_states_at`, they are the
                requested instants (each an accepted-step endpoint — intermediate accepted steps are not
                recorded). Contributions are expressed in GCRF, the integration frame.

                Returns:
                    Propagator.StepContributions: The recorded step contributions.

            )doc"
        )
        .def(
            "set_dynamics",
            &Propagator::setDynamics,
            arg("dynamics"),
            R"doc(
                Set the dynamics.

                Args:
                    dynamics (list[Dynamics]) The dynamics.

            )doc"
        )
        .def(
            "add_dynamics",
            &Propagator::addDynamics,
            arg("dynamics"),
            R"doc(
                Add dynamics.

                Args:
                    dynamics (Dynamics) The dynamics.

            )doc"
        )
        .def(
            "clear_dynamics",
            &Propagator::clearDynamics,
            R"doc(
                Clear the dynamics.

            )doc"
        )

        .def(
            "add_maneuver",
            &Propagator::addManeuver,
            arg("maneuver"),
            arg_v(
                "interpolation_type",
                DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE,
                "Interpolator.Type.BarycentricRational"
            ),
            R"doc(
                Add a maneuver.

                Args:
                    maneuver (Maneuver) The maneuver.
                    interpolation_type (Interpolator.Type, optional) The interpolation type. Defaults to Barycentric Rational.

            )doc"
        )

        .def(
            "set_contribution_observation_enabled",
            &Propagator::setContributionObservationEnabled,
            arg("enabled"),
            R"doc(
                Enable or disable per-dynamics step contribution recording (disabled by default).

                When enabled, each `calculate_*` call additionally evaluates every dynamics once per recorded
                step (one extra right-hand-side-equivalent evaluation per step) and stores the results. Memory
                grows linearly with the number of recorded steps.

                Args:
                    enabled (bool) True to enable recording, False to disable it.

            )doc"
        )

        .def(
            "calculate_state_at",
            &Propagator::calculateStateAt,
            arg("state"),
            arg("instant"),
            R"doc(
                Calculate the state at a given instant.

                Args:
                    state (State) The state.
                    instant (Instant) The instant.

                Returns:
                    State: The state at the given instant.

            )doc"
        )
        .def(
            "calculate_state_to_condition",
            &Propagator::calculateStateToCondition,
            arg("state"),
            arg("instant"),
            arg("event_condition"),
            R"doc(
                Calculate the state up to a given event condition.

                Args:
                    state (State) The state.
                    instant (Instant) The instant.
                    event_condition (EventCondition) The event condition.

                Returns:
                    State: The state up to the given event condition.

            )doc"
        )

        .def(
            "calculate_states_at",
            &Propagator::calculateStatesAt,
            arg("state"),
            arg("instants"),
            R"doc(
                Calculate the states at given instants. It is more performant than looping `calculate_state_at` for multiple instants.

                Args:
                    state (State) The state.
                    instants (list[Instant]) The instants.

                Returns:
                    list[State]: The states at the given instants.

            )doc"
        )

        .def_static(
            "default",
            overload_cast<>(&Propagator::Default),
            R"doc(
                Get the default propagator.

                Returns:
                    Propagator: The default propagator.
            )doc"
        )
        .def_static(
            "default",
            overload_cast<const Environment&>(&Propagator::Default),
            arg("environment"),
            R"doc(
                Get the default propagator for a given environment.

                Args:
                    environment (Environment) The environment.

                Returns:
                    Propagator: The default propagator for the given environment.
            )doc"
        )
        .def_static(
            "from_environment",
            &Propagator::FromEnvironment,
            arg("numerical_solver"),
            arg("environment"),
            R"doc(
                Create a propagator from an environment.

                Args:
                    numerical_solver (NumericalSolver) The numerical solver.
                    environment (Environment) The environment.

                Returns:
                    Propagator: The propagator.
            )doc"
        );
}
