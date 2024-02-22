/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::container::Array;

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

    class_<Propagator>(
        aModule,
        "Propagator",
        R"doc(
            A `Propagator` that propagates the provided `State` using it's `NumericalSolver` under the set `Dynamics`.

        )doc"
    )

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
            arg("interpolation_type") = DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE,
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
            arg("interpolation_type") = DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE,
            R"doc(
                Add a maneuver.

                Args:
                    maneuver (Maneuver) The maneuver.
                    interpolation_type (Interpolator.Type, optional) The interpolation type. Defaults to Barycentric Rational.

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
