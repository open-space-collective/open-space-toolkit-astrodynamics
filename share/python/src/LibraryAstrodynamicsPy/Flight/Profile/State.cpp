////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Profile/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Flight_Profile_State ( )
{
    
    using namespace boost::python ;

    using library::core::types::Shared ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::Quaternion ;

    using library::physics::time::Instant ;
    using library::physics::coord::Frame ;
    
    using library::astro::flight::profile::State ;

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
        // .def("inFrame", &State::inFrame)
        
        .def("Undefined", &State::Undefined).staticmethod("Undefined")

    ;

    LibraryAstrodynamicsPy_Flight_Profile_State() ;
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////