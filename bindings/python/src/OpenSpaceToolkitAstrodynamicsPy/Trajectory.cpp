////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory  (           pybind11::module&           aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::ctnr::Array ;

    using ostk::astro::Trajectory ;
    using ostk::astro::trajectory::State ;

    class_<Trajectory>(aModule, "Trajectory")

        .def(init<const ostk::astro::trajectory::Model&>())

        .def(init<const Array<State>&>())

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Trajectory>))
        .def("__repr__", &(shiftToString<Trajectory>))

        .def("is_defined", &Trajectory::isDefined)

        // .def("access_model", &Trajectory::accessModel, return_value_policy<reference_existing_object>())

        .def("get_state_at", &Trajectory::getStateAt)
        .def("get_states_at", &Trajectory::getStatesAt)

        .def_static("undefined", &Trajectory::Undefined)
        .def_static("position", &Trajectory::Position)

    ;

    // Create "trajectory" python submodule
    auto trajectory = aModule.def_submodule("trajectory") ;

    // Add __path__ attribute for "trajectory" submodule
    trajectory.attr("__path__") = "ostk.astrodynamics.trajectory" ;

    // Add objects to python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(trajectory) ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit(trajectory) ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(trajectory) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
