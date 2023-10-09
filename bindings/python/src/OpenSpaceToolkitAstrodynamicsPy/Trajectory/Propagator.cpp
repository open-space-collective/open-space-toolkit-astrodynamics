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

    class_<Propagator>(aModule, "Propagator")

        .def(
            init<const NumericalSolver&, const Array<Shared<Dynamics>>&>(),
            arg("numerical_solver"),
            arg("dynamics") = Array<Shared<Dynamics>>::Empty()
        )

        .def("__str__", &(shiftToString<Propagator>))
        .def("__repr__", &(shiftToString<Propagator>))

        .def("is_defined", &Propagator::isDefined)

        .def("access_numerical_solver", &Propagator::accessNumericalSolver)

        .def("get_number_of_coordinates", &Propagator::getNumberOfCoordinates)
        .def("get_dynamics", &Propagator::getDynamics)
        .def("set_dynamics", &Propagator::setDynamics, arg("dynamics"))
        .def("add_dynamics", &Propagator::addDynamics, arg("dynamics"))
        .def("clear_dynamics", &Propagator::clearDynamics)

        .def("calculate_state_at", &Propagator::calculateStateAt, arg("state"), arg("instant"))
        .def(
            "calculate_state_to_condition",
            &Propagator::calculateStateToCondition,
            arg("state"),
            arg("instant"),
            arg("event_condition")
        )

        .def("calculate_states_at", &Propagator::calculateStatesAt, arg("state"), arg("instant_array"))

        .def_static("default", overload_cast<>(&Propagator::Default))
        .def_static("default", overload_cast<const Environment&>(&Propagator::Default), arg("environment"))
        .def_static("from_environment", &Propagator::FromEnvironment, arg("numerical_solver"), arg("environment"));
}
