////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Access.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Access/Generator.cpp>
#include <LibraryAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <Library/Astrodynamics/Access.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Access        	    ( )
{
    
    using namespace boost::python ;

    using library::physics::time::Instant ;

    using library::astro::Access ;

    scope in_Access = class_<Access>("Access", init<const Access::Type&, const Instant&, const Instant&, const Instant&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Access::isDefined)
        .def("isComplete", &Access::isComplete)
        
        .def("getType", &Access::getType)
        .def("getAcquisitionOfSignal", &Access::getAcquisitionOfSignal)
        .def("getTimeOfClosestApproach", &Access::getTimeOfClosestApproach)
        .def("getLossOfSignal", &Access::getLossOfSignal)
        .def("getInterval", &Access::getInterval)
        .def("getDuration", &Access::getDuration)

        .def("Undefined", &Access::Undefined).staticmethod("Undefined")
        
        .def("StringFromType", &Access::StringFromType).staticmethod("StringFromType")

    ;

    enum_<Access::Type>("Type")

        .value("Undefined", Access::Type::Undefined)
        .value("Complete", Access::Type::Complete)
        .value("Partial", Access::Type::Partial)

    ;

    using library::core::ctnr::Array ;

    IterableConverter()

        .from_python<Array<Access>>()
        .to_python<Array<Access>>()
        
    ;

    LibraryAstrodynamicsPy_Access_Generator() ;
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////