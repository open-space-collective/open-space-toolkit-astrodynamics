////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access/Generator.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Access_Generator     ( )
{
    
    using namespace boost::python ;

    using library::physics::Environment ;
    using library::physics::coord::spherical::AER ;

    using library::astro::Access ;
    using library::astro::access::Generator ;

    scope in_Generator = class_<Generator>("Generator", init<const Environment&>())

        .def("isDefined", &Generator::isDefined)
        
        .def("computeAccesses", &Generator::computeAccesses)
        .def("setAerFilter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAerFilter(object) ; })
        .def("setAccessFilter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAccessFilter(object) ; })

        .def("Undefined", &Generator::Undefined).staticmethod("Undefined")
        .def("AerRanges", &Generator::AerRanges).staticmethod("AerRanges")

    ;
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////