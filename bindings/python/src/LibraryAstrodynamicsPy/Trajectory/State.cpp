////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_State     ( )
{

    using namespace boost::python ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::astro::trajectory::State ;

    scope in_State = class_<State>("State", init<const Instant&, const Position&, const Velocity&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &State::isDefined)

        .def("getInstant", &State::getInstant)
        .def("getPosition", &State::getPosition)
        .def("getVelocity", &State::getVelocity)
        .def("inFrame", &State::inFrame)

        .def("Undefined", &State::Undefined).staticmethod("Undefined")

    ;

    using ostk::core::ctnr::Array ;

    IterableConverter()

        .from_python<Array<State>>()
        .to_python<Array<State>>()

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
