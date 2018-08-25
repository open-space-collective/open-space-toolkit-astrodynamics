////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Model.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Model ( )
{

    using namespace boost::python ;

    using library::astro::trajectory::orbit::Model ;

    // scope in_Model = class_<Model, bases<library::astro::trajectory::Model>, boost::noncopyable>("Model", no_init)
    scope in_Model = class_<Model, boost::noncopyable>("OrbitModel", no_init)

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))

        .def("isDefined", &Model::isDefined)

        .def("getEpoch", &Model::getEpoch)
        .def("getRevolutionNumberAtEpoch", &Model::getRevolutionNumberAtEpoch)
        .def("calculateStateAt", &Model::calculateStateAt)
        .def("calculateRevolutionNumberAt", &Model::calculateRevolutionNumberAt)
        .def("print", &Model::print)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////