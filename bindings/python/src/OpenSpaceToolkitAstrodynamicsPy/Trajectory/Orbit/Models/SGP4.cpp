////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4/TLE.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4 ( )
{

    using namespace boost::python ;

    using ostk::astro::trajectory::orbit::models::SGP4 ;
    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        scope in_SGP4 = class_<SGP4, bases<ostk::astro::trajectory::orbit::Model>>("SGP4", init<TLE>())
        // scope in_SGP4 = class_<SGP4, bases<ostk::astro::trajectory::orbit::Model>>("SGP4", init<TLE>())

            .def(self == self)
            .def(self != self)

            .def(self_ns::str(self_ns::self))
            .def(self_ns::repr(self_ns::self))

            .def("is_defined", &SGP4::isDefined)

            .def("get_tle", &SGP4::getTle)
            .def("get_epoch", &SGP4::getEpoch)
            .def("get_revolution_number_at_epoch", &SGP4::getRevolutionNumberAtEpoch)
            .def("calculate_state_at", &SGP4::calculateStateAt)
            .def("calculate_revolution_number_at", &SGP4::calculateRevolutionNumberAt)

        ;

    }

    boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("ostk.astrodynamics.trajectory.orbit.models.sgp4")))) ;

    boost::python::scope().attr("sgp4") = module ;

    boost::python::scope scope = module ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
