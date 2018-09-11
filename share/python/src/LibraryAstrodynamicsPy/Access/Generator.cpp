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

    using library::core::types::Shared ;
    
    using library::physics::Environment ;
    using library::physics::coord::spherical::AER ;

    using library::astro::Access ;
    using library::astro::access::Generator ;

    scope in_Generator = class_<Generator>("Generator", init<const Environment&>())

        .def
        (
            "__init__",
            make_constructor
            (
                +[] (const Environment& anEnvironment, const boost::python::object& anAerFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter) ;
                }
            )
        )

        .def
        (
            "__init__",
            make_constructor
            (
                +[] (const Environment& anEnvironment, const boost::python::object& anAerFilter, const boost::python::object& anAccessFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter, anAccessFilter) ;
                }
            )
        )

        .def("isDefined", &Generator::isDefined)
        
        .def("computeAccesses", &Generator::computeAccesses)
        .def("setStep", &Generator::setStep)
        .def("setTolerance", &Generator::setTolerance)
        .def("setAerFilter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAerFilter(object) ; })
        .def("setAccessFilter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAccessFilter(object) ; })

        .def("Undefined", &Generator::Undefined).staticmethod("Undefined")
        .def("AerRanges", &Generator::AerRanges).staticmethod("AerRanges")

    ;
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////