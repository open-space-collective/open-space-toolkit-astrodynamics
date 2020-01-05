////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <Library/Astrodynamics/Flight/Profile/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Flight_Profile_State ( )
{

    using namespace boost::python ;

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    scope in_State = class_<State>("State", init<const Instant&, const Vector3d&, const Vector3d&, const Quaternion&, const Vector3d&, const Shared<const Frame>&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &State::isDefined)

        .def("getInstant", &State::getInstant)
        .def("getPosition", &State::getPosition)
        .def("getVelocity", &State::getVelocity)
        .def("getAttitude", &State::getAttitude)
        .def("getAngularVelocity", &State::getAngularVelocity)
        .def("getFrame", &State::getFrame)
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
