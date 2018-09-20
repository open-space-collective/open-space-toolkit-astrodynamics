////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Trajectory/Model.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/Orbit.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/State.cpp>

#include <Library/Astrodynamics/Trajectory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory        	( )
{
    
    using namespace boost::python ;

    using library::core::ctnr::Array ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;

    scope in_Trajectory = class_<Trajectory>("Trajectory", init<const library::astro::trajectory::Model&>())

        .def(init<const Array<State>&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Trajectory::isDefined)

        .def("getStateAt", &Trajectory::getStateAt)
        .def("getStatesAt", &Trajectory::getStatesAt)
        
        .def("Undefined", &Trajectory::Undefined).staticmethod("Undefined")
        .def("Position", &Trajectory::Position).staticmethod("Position")

    ;
    
    LibraryAstrodynamicsPy_Trajectory_State() ;
    LibraryAstrodynamicsPy_Trajectory_Orbit() ;
    LibraryAstrodynamicsPy_Trajectory_Model() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////