/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::astro::NumericalSolver;
    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::trajectory::Propagator;

    class_<Propagator>(aModule, "Propagator")

        .def(
            init<const NumericalSolver&, const Array<Shared<Dynamics>>&>(),
            arg("numerical_solver"),
            arg("dynamics") = Array<Shared<Dynamics>>::Empty()
        )

        .def("__str__", &(shiftToString<Propagator>))
        .def("__repr__", &(shiftToString<Propagator>))

        .def("is_defined", &Propagator::isDefined)

        .def("get_dynamics", &Propagator::getDynamics)
        .def("set_dynamics", &Propagator::setDynamics, arg("dynamics"))
        .def("add_dynamics", &Propagator::addDynamics, arg("dynamics"))
        .def("clear_dynamics", &Propagator::clearDynamics)

        .def("calculate_state_at", &Propagator::calculateStateAt, arg("state"), arg("instant"))

        .def("calculate_states_at", &Propagator::calculateStatesAt, arg("state"), arg("instant_array"));
}
