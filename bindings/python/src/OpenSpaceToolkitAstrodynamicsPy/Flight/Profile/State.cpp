////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State ( )
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

        .def("is_defined", &State::isDefined)

        .def("get_instant", &State::getInstant)
        .def("get_position", &State::getPosition)
        .def("get_velocity", &State::getVelocity)
        .def("get_attitude", &State::getAttitude)
        .def("get_angular_velocity", &State::getAngularVelocity)
        .def("get_frame", &State::getFrame)
        .def("in_frame", &State::inFrame)

        .def("undefined", &State::Undefined).staticmethod("undefined")

    ;

    using ostk::core::ctnr::Array ;

    IterableConverter()

        .from_python<Array<State>>()
        .to_python<Array<State>>()

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
