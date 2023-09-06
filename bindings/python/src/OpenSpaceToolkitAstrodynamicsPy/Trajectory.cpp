/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameDirection.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameFactory.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameTransformProvider.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Propagator.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/TrajectorySegment.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;

    using ostk::astro::Trajectory;
    using ostk::astro::trajectory::State;

    class_<Trajectory>(aModule, "Trajectory")

        .def(init<const ostk::astro::trajectory::Model&>(), arg("model"))
        .def(init<const Array<State>&>(), arg("states"))

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Trajectory>))
        .def("__repr__", &(shiftToString<Trajectory>))

        .def("is_defined", &Trajectory::isDefined)

        // .def("access_model", &Trajectory::accessModel, return_value_policy<reference_existing_object>())

        .def("get_state_at", &Trajectory::getStateAt, arg("instant"))
        .def("get_states_at", &Trajectory::getStatesAt, arg("instants"))

        .def_static("undefined", &Trajectory::Undefined)
        .def_static("position", &Trajectory::Position, arg("position"))

        ;

    // Create "trajectory" python submodule
    auto trajectory = aModule.def_submodule("trajectory");

    // Add __path__ attribute for "trajectory" submodule
    trajectory.attr("__path__") = "ostk.astrodynamics.trajectory";

    // Add objects to python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameTransformProvider(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameFactory(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameDirection(trajectory);

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(trajectory);
}
