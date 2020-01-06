////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/Profile.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/State.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_Profile ( )
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

        .def("is_defined", &Profile::isDefined)

        .def("get_state_at", &Profile::getStateAt)
        .def("get_states_at", &Profile::getStatesAt)
        .def("get_axes_at", &Profile::getAxesAt)

        .def("undefined", &Profile::Undefined).staticmethod("undefined")
        .def("inertial_pointing", &Profile::InertialPointing).staticmethod("inertial_pointing")
        .def("nadir_pointing", &Profile::NadirPointing).staticmethod("nadir_pointing")

    ;

    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
