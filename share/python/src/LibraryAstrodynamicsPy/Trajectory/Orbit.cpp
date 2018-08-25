////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Pass.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Models.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Model.cpp>

#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit     ( )
{

    using namespace boost::python ;

    using library::astro::trajectory::Orbit ;

    scope in_Orbit = class_<Orbit, bases<library::astro::Trajectory>>("Orbit", init<const library::astro::trajectory::orbit::Model&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Orbit::isDefined)

        .def("getRevolutionNumberAt", &Orbit::getRevolutionNumberAt)
        .def("getPassAt", &Orbit::getPassAt)
        .def("getPassWithRevolutionNumber", &Orbit::getPassWithRevolutionNumber)
        .def("print", &Orbit::print)
        
        .def("Undefined", &Orbit::Undefined).staticmethod("Undefined")

    ;

    LibraryAstrodynamicsPy_Trajectory_Orbit_Model() ;
    LibraryAstrodynamicsPy_Trajectory_Orbit_Models() ;
    LibraryAstrodynamicsPy_Trajectory_Orbit_Pass() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////