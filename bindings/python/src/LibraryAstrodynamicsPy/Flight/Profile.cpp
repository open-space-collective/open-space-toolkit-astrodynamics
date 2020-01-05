////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Flight/Profile.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Flight/Profile/State.cpp>

#include <Library/Astrodynamics/Flight/Profile.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Flight_Profile       ( )
{

    using namespace boost::python ;

    using ostk::core::types::Shared ;
    using ostk::core::ctnr::Array ;

    using ostk::physics::coord::Frame ;
    using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

    scope in_Profile = class_<Profile>("Profile", init<const DynamicProvider&, const Shared<const Frame>&>())

        // .def(init<const Array<State>&>())

        // .def(self == self)
        // .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Profile::isDefined)

        .def("getStateAt", &Profile::getStateAt)
        .def("getStatesAt", &Profile::getStatesAt)
        .def("getAxesAt", &Profile::getAxesAt)

        .def("Undefined", &Profile::Undefined).staticmethod("Undefined")
        .def("InertialPointing", &Profile::InertialPointing).staticmethod("InertialPointing")
        .def("NadirPointing", &Profile::NadirPointing).staticmethod("NadirPointing")

    ;

    LibraryAstrodynamicsPy_Flight_Profile_State() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
