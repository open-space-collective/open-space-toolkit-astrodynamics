////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model.cpp>
#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit.cpp>
#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/State.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory  ( )
{

    using namespace boost::python ;

    using ostk::core::ctnr::Array ;

    using ostk::astro::Trajectory ;
    using ostk::astro::trajectory::State ;

    scope in_Trajectory = class_<Trajectory>("Trajectory", init<const ostk::astro::trajectory::Model&>())

        .def(init<const Array<State>&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Trajectory::isDefined)

        // .def("accessModel", &Trajectory::accessModel, return_value_policy<reference_existing_object>())

        .def("getStateAt", &Trajectory::getStateAt)
        .def("getStatesAt", &Trajectory::getStatesAt)

        .def("Undefined", &Trajectory::Undefined).staticmethod("Undefined")
        .def("Position", &Trajectory::Position).staticmethod("Position")

    ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
