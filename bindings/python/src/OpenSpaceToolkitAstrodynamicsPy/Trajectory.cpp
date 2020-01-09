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

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory  ( )
{

    using namespace boost::python ;

    using ostk::core::ctnr::Array ;

    using ostk::astro::Trajectory ;
    using ostk::astro::trajectory::State ;

    class_<Trajectory>("Trajectory", init<const ostk::astro::trajectory::Model&>())

        .def(init<const Array<State>&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("is_defined", &Trajectory::isDefined)

        // .def("access_model", &Trajectory::accessModel, return_value_policy<reference_existing_object>())

        .def("get_state_at", &Trajectory::getStateAt)
        .def("get_states_at", &Trajectory::getStatesAt)

        .def("undefined", &Trajectory::Undefined).staticmethod("undefined")
        .def("position", &Trajectory::Position).staticmethod("position")

    ;

    boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("ostk.astrodynamics.trajectory")))) ;

    boost::python::scope().attr("trajectory") = module ;

    boost::python::scope scope = module ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
