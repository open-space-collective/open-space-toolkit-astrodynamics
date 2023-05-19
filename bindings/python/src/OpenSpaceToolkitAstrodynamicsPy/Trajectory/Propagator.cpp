/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;
    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::trajectory::Propagator;
    using ostk::astro::trajectory::State;

    class_<Propagator>(aModule, "Propagator")

        .def(init<const Array<Shared<Dynamics>>&, const NumericalSolver&>(), arg("dynamics"), arg("numerical_solver"))

        .def("__str__", &(shiftToString<Propagator>))
        .def("__repr__", &(shiftToString<Propagator>))

        .def("is_defined", &Propagator::isDefined)

        .def("calculate_state_at", &Propagator::calculateStateAt, arg("state"), arg("instant"))

        .def("calculate_states_at", &Propagator::calculateStatesAt, arg("state"), arg("instant_array"))

        ;
}
