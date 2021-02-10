////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Pass.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass (        pybind11::module&    aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::types::Integer ;

    using ostk::physics::time::Interval ;

    using ostk::astro::trajectory::orbit::Pass ;

    class_<Pass> pass_class(aModule, "Pass") ;

    pass_class.def(init<const Pass::Type&, const Integer&, const Interval&>())

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Pass>))
        .def("__repr__", &(shiftToString<Pass>))

        .def("is_defined", &Pass::isDefined)
        .def("is_complete", &Pass::isComplete)

        .def("get_type", &Pass::getType)
        .def("get_revolution_number", &Pass::getRevolutionNumber)
        .def("get_interval", &Pass::getInterval)

        .def_static("undefined", &Pass::Undefined)
        .def_static("string_from_type", &Pass::StringFromType)
        .def_static("string_from_phase", &Pass::StringFromPhase)
        .def_static("string_from_quarter", &Pass::StringFromQuarter)

    ;

    enum_<Pass::Type>(pass_class, "Type")

        .value("Undefined", Pass::Type::Undefined)
        .value("Complete", Pass::Type::Complete)
        .value("Partial", Pass::Type::Partial)

    ;

    enum_<Pass::Phase>(pass_class, "Phase")

        .value("Undefined", Pass::Phase::Undefined)
        .value("Ascending", Pass::Phase::Ascending)
        .value("Descending", Pass::Phase::Descending)

    ;

    enum_<Pass::Quarter>(pass_class, "Quarter")

        .value("Undefined", Pass::Quarter::Undefined)
        .value("First", Pass::Quarter::First)
        .value("Second", Pass::Quarter::Second)
        .value("Third", Pass::Quarter::Third)
        .value("Fourth", Pass::Quarter::Fourth)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
