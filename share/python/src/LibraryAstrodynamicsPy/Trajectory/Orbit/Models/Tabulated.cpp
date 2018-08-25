////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Models/Tabulated.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Models_Tabulated ( )
{

    using namespace boost::python ;

    using library::core::types::Integer ;
    using library::core::ctnr::Array ;

    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Tabulated ;

    scope in_Tabulated = class_<Tabulated>("Tabulated", init<Array<State>, Integer>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Tabulated::isDefined)

        .def("getEpoch", &Tabulated::getEpoch)
        .def("getRevolutionNumberAtEpoch", &Tabulated::getRevolutionNumberAtEpoch)
        .def("calculateStateAt", &Tabulated::calculateStateAt)
        .def("calculateRevolutionNumberAt", &Tabulated::calculateRevolutionNumberAt)
        .def("print", &Tabulated::print)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////