////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Pass.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Pass.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Pass ( )
{

    using namespace boost::python ;

    using library::core::types::Integer ;

    using library::physics::time::Interval ;

    using library::astro::trajectory::orbit::Pass ;

    scope in_Pass = class_<Pass>("Pass", init<const Pass::Type&, const Integer&, const Interval&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Pass::isDefined)
        .def("isComplete", &Pass::isComplete)

        .def("getType", &Pass::getType)
        .def("getRevolutionNumber", &Pass::getRevolutionNumber)
        .def("getInterval", &Pass::getInterval)

        .def("Undefined", &Pass::Undefined).staticmethod("Undefined")
        .def("StringFromType", &Pass::StringFromType).staticmethod("StringFromType")
        .def("StringFromPhase", &Pass::StringFromPhase).staticmethod("StringFromPhase")
        .def("StringFromQuarter", &Pass::StringFromQuarter).staticmethod("StringFromQuarter")

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
