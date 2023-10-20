/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::physics::Environment;
    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::state::NumericalSolver;
    using ostk::astro::EventCondition;
    using ostk::astro::Dynamics;
    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::trajectory::Propagator;
    using ostk::astro::trajectory::State;

    class_<Propagator>(
        aModule,
        "Propagator",
        R"mydelimiter(
            A `Propagator` that proapgates the provided `State` using it's `NumericalSolver` under the set `Dynamics`.

            Group:
                trajectory
        )mydelimiter"
    )

        .def(
            init<const NumericalSolver&, const Array<Shared<Dynamics>>&>(),
            arg("numerical_solver"),
            arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
            R"mydelimiter(
                Construct a new `Propagator` object.

                Args:
                    numerical_solver (NumericalSolver) The numerical solver.
                    dynamics (Array[Shared[Dynamics]], optional) The dynamics.

                Returns:
                    Propagator: The new `Propagator` object.

            )mydelimiter"
        )

        .def("__str__", &(shiftToString<Propagator>))
        .def("__repr__", &(shiftToString<Propagator>))

        .def("is_defined", &Propagator::isDefined,
            R"mydelimiter(
                Check if the propagator is defined.

                Returns:
                    bool: True if the propagator is defined, False otherwise.

            )mydelimiter"
        )

        .def("access_numerical_solver", &Propagator::accessNumericalSolver,
            R"mydelimiter(
                Access the numerical solver.

                Returns:
                    NumericalSolver&: The numerical solver.

            )mydelimiter"
        )

        .def("get_number_of_coordinates", &Propagator::getNumberOfCoordinates,
            R"mydelimiter(
                Get the number of coordinates.

                Returns:
                    int: The number of coordinates.
                
            )mydelimiter"
        )
        .def("get_dynamics", &Propagator::getDynamics,
            R"mydelimiter(
                Get the dynamics.

                Returns:
                    Array[Shared[Dynamics]]: The dynamics.
                
            )mydelimiter"
        )
        .def("set_dynamics", &Propagator::setDynamics, arg("dynamics"),
            R"mydelimiter(
                Set the dynamics.

                Args:
                    dynamics (Array[Shared[Dynamics]]) The dynamics.
                
            )mydelimiter"
        )
        .def("add_dynamics", &Propagator::addDynamics, arg("dynamics"),
            R"mydelimiter(
                Add dynamics.

                Args:
                    dynamics (Shared[Dynamics]) The dynamics.
                
            )mydelimiter"
        )
        .def("clear_dynamics", &Propagator::clearDynamics,
            R"mydelimiter(
                Clear the dynamics.
            
            )mydelimiter"
        )

        .def("calculate_state_at", &Propagator::calculateStateAt, arg("state"), arg("instant"),
            R"mydelimiter(
                Calculate the state at a given instant.

                Args:
                    state (State) The state.
                    instant (Instant) The instant.

                Returns:
                    State: The state at the given instant.
                
            )mydelimiter"
        )
        .def(
            "calculate_state_to_condition",
            &Propagator::calculateStateToCondition,
            arg("state"),
            arg("instant"),
            arg("event_condition"),
            R"mydelimiter(
                Calculate the state up to a given event condition.

                Args:
                    state (State) The state.
                    instant (Instant) The instant.
                    event_condition (EventCondition) The event condition.

                Returns:
                    State: The state up to the given event condition.
                
            )mydelimiter"
        )

        .def("calculate_states_at", &Propagator::calculateStatesAt, arg("state"), arg("instant_array"),
            R"mydelimiter(
                Calculate the states at given instants.

                Args:
                    state (State) The state.
                    instant_array (Array[Instant]) The instants.

                Returns:
                    Array[State]: The states at the given instants.
                
            )mydelimiter"
        )

        .def_static("default", overload_cast<>(&Propagator::Default),
            R"mydelimiter(
                Get the default propagator.

                Returns:
                    Propagator: The default propagator.
                
            )mydelimiter"
        )
        .def_static("default", overload_cast<const Environment&>(&Propagator::Default), arg("environment"),
            R"mydelimiter(
                Get the default propagator for a given environment.

                Args:
                    environment (Environment) The environment.

                Returns:
                    Propagator: The default propagator for the given environment.
                
            )mydelimiter"
        )
        .def_static("from_environment", &Propagator::FromEnvironment, arg("numerical_solver"), arg("environment"),
            R"mydelimiter(
                Create a propagator from an environment.

                Args:
                    numerical_solver (NumericalSolver) The numerical solver.
                    environment (Environment) The environment.

                Returns:
                    Propagator: The propagator.

                Group:
                    Static methods
            )mydelimiter"
        );
}