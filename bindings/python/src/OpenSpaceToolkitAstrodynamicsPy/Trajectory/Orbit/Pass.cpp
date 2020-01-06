////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Pass.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass ( )
{

    using namespace boost::python ;

    using ostk::core::types::Integer ;

    using ostk::physics::time::Interval ;

    using ostk::astro::trajectory::orbit::Pass ;

    scope in_Pass = class_<Pass>("Pass", init<const Pass::Type&, const Integer&, const Interval&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("is_defined", &Pass::isDefined)
        .def("is_complete", &Pass::isComplete)

        .def("get_type", &Pass::getType)
        .def("get_revolution_number", &Pass::getRevolutionNumber)
        .def("get_interval", &Pass::getInterval)

        .def("undefined", &Pass::Undefined).staticmethod("undefined")
        .def("string_from_type", &Pass::StringFromType).staticmethod("string_from_type")
        .def("string_from_phase", &Pass::StringFromPhase).staticmethod("string_from_phase")
        .def("string_from_quarter", &Pass::StringFromQuarter).staticmethod("string_from_quarter")

    ;

    enum_<Pass::Type>("Type")

        .value("Undefined", Pass::Type::Undefined)
        .value("Complete", Pass::Type::Complete)
        .value("Partial", Pass::Type::Partial)

    ;

    enum_<Pass::Phase>("Phase")

        .value("Undefined", Pass::Phase::Undefined)
        .value("Ascending", Pass::Phase::Ascending)
        .value("Descending", Pass::Phase::Descending)

    ;

    enum_<Pass::Quarter>("Quarter")

        .value("Undefined", Pass::Quarter::Undefined)
        .value("First", Pass::Quarter::First)
        .value("Second", Pass::Quarter::Second)
        .value("Third", Pass::Quarter::Third)
        .value("Fourth", Pass::Quarter::Fourth)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
