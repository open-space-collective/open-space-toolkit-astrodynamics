////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Access      (           pybind11::module&           aModule                                     )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;

    using ostk::astro::Access ;

    {

        class_<Access> access_class(aModule, "Access") ;

        access_class.def(init<const Access::Type&, const Instant&, const Instant&, const Instant&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Access>))
            .def("__repr__", &(shiftToString<Access>))

            .def("is_defined", &Access::isDefined)
            .def("is_complete", &Access::isComplete)

            .def("get_type", &Access::getType)
            .def("get_acquisition_of_signal", &Access::getAcquisitionOfSignal)
            .def("get_time_of_closest_approach", &Access::getTimeOfClosestApproach)
            .def("get_loss_of_signal", &Access::getLossOfSignal)
            .def("get_interval", &Access::getInterval)
            .def("get_duration", &Access::getDuration)

            .def_static("undefined", &Access::Undefined)

            .def_static("string_from_type", &Access::StringFromType)

        ;

        enum_<Access::Type>(access_class, "Type")

            .value("Undefined", Access::Type::Undefined)
            .value("Complete", Access::Type::Complete)
            .value("Partial", Access::Type::Partial)

        ;

    }

    // Create "access" python submodule
    auto access = aModule.def_submodule("access") ;

    // Add __path__ attribute for "access" submodule
    access.attr("__path__") = "ostk.astrodynamics.access" ;

    // Add elements to "access" module
    OpenSpaceToolkitAstrodynamicsPy_Access_Generator(access) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
