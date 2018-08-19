////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Models/SGP4.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Models/SGP4/TLE.cpp>

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Models_SGP4 ( )
{

    using namespace boost::python ;

    using library::astro::trajectory::orbit::models::SGP4 ;
    using library::astro::trajectory::orbit::models::sgp4::TLE ;

    scope in_SGP4 = class_<SGP4>("SGP4", init<TLE>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))

        .def("isDefined", &SGP4::isDefined)

        .def("getTle", &SGP4::getTle)
        .def("getEpoch", &SGP4::getEpoch)
        .def("getRevolutionNumberAtEpoch", &SGP4::getRevolutionNumberAtEpoch)
        .def("calculateStateAt", &SGP4::calculateStateAt)
        .def("calculateRevolutionNumberAt", &SGP4::calculateRevolutionNumberAt)
        .def("print", &SGP4::print)

    ;

    LibraryAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////