////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp>
#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Access      ( )
{

    using namespace boost::python ;

    using ostk::physics::time::Instant ;

    using ostk::astro::Access ;

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

    using ostk::core::ctnr::Array ;

    IterableConverter()

        .from_python<Array<Access>>()
        .to_python<Array<Access>>()

    ;

    OpenSpaceToolkitAstrodynamicsPy_Access_Generator() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
