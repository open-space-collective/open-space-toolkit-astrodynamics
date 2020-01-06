////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/IterableConverter.hpp>

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

        .def("is_defined", &Access::isDefined)
        .def("is_complete", &Access::isComplete)

        .def("get_type", &Access::getType)
        .def("get_acquisition_of_signal", &Access::getAcquisitionOfSignal)
        .def("get_time_of_closest_approach", &Access::getTimeOfClosestApproach)
        .def("get_loss_of_signal", &Access::getLossOfSignal)
        .def("get_interval", &Access::getInterval)
        .def("get_duration", &Access::getDuration)

        .def("undefined", &Access::Undefined).staticmethod("undefined")

        .def("string_from_type", &Access::StringFromType).staticmethod("string_from_type")

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
