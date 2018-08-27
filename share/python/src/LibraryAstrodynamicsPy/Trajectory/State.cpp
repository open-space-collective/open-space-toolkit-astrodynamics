////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <Library/Astrodynamics/Trajectory/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_State     ( )
{

    using namespace boost::python ;

    using library::physics::time::Instant ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    
    using library::astro::trajectory::State ;

    scope in_State = class_<State>("State", init<const Instant&, const Position&, const Velocity&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &State::isDefined)

        .def("accessInstant", &State::accessInstant, return_value_policy<reference_existing_object>())
        .def("accessPosition", &State::accessPosition, return_value_policy<reference_existing_object>())
        .def("accessVelocity", &State::accessVelocity, return_value_policy<reference_existing_object>())
        .def("inFrame", &State::inFrame)
        
        .def("Undefined", &State::Undefined).staticmethod("Undefined")

    ;

    using library::core::ctnr::Array ;

    IterableConverter()

        .from_python<Array<State>>()
        .to_python<Array<State>>()
        
    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////