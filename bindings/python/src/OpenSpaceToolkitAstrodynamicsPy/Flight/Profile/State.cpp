////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State (        pybind11::module&     aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    class_<State>(aModule, "State")

        .def
        (
            init<const Instant&, const Vector3d&, const Vector3d&, const Quaternion&, const Vector3d&, const Shared<const Frame>&>(),
            arg("instant"),
            arg("position"),
            arg("velocity"),
            arg("attitude"),
            arg("angular_velocity"),
            arg("reference_frame")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def("is_defined", &State::isDefined)

        .def("get_instant", &State::getInstant)
        .def("get_position", &State::getPosition)
        .def("get_velocity", &State::getVelocity)
        .def("get_attitude", &State::getAttitude)
        .def("get_angular_velocity", &State::getAngularVelocity)
        .def("get_frame", &State::getFrame)
        .def("in_frame", &State::inFrame, arg("frame"))

        .def_static("undefined", &State::Undefined)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
