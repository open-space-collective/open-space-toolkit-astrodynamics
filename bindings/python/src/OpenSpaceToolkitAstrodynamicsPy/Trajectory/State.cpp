/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesBroker.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubset.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/NumericalSolver.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::coord::Frame;
    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::state::CoordinatesBroker;

    class_<State>(aModule, "State")

        .def(init<const Instant&, const Position&, const Velocity&>(), arg("instant"), arg("position"), arg("velocity"))
        .def(
            init<const Instant&, const VectorXd&, const Shared<const Frame>&, const Shared<const CoordinatesBroker>&>(),
            arg("instant"),
            arg("coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )

        .def(self == self)
        .def(self != self)
        .def(self + self)
        .def(self - self)

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def("is_defined", &State::isDefined)

        .def("get_size", &State::getSize)
        .def("get_instant", &State::getInstant)
        .def("get_position", &State::getPosition)
        .def("get_velocity", &State::getVelocity)
        .def("get_coordinates", &State::getCoordinates)
        .def("get_coordinates_subsets", &State::getCoordinatesSubsets)
        .def("get_frame", &State::getFrame)

        .def("extract_coordinates", &State::extractCoordinates, arg("coordinates_subset"))

        .def("in_frame", &State::inFrame, arg("frame"))

        .def_static("undefined", &State::Undefined)

        ;

    // Create "state" python submodule
    auto state = aModule.def_submodule("state");

    // Add __path__ attribute for "state" submodule
    state.attr("__path__") = "ostk.astrodynamics.trajectory.state";

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesBroker(state);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubset(state);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_NumericalSolver(state);
}
