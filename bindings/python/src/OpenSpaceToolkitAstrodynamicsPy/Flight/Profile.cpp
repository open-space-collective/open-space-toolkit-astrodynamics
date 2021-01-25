////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/Profile.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/State.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_Profile (        pybind11::module&           aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::types::Shared ;
    using ostk::core::ctnr::Array ;

    using ostk::physics::coord::Frame ;
    using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

    class_<Profile>(aModule, "Profile")

        .def(init<const DynamicProvider&, const Shared<const Frame>&>())

        // .def(init<const Array<State>&>())

        // .def(self == self)
        // .def(self != self)

        // .def(self_ns::str(self_ns::self))
        // .def(self_ns::repr(self_ns::self))

        .def("is_defined", &Profile::isDefined)

        .def("get_state_at", &Profile::getStateAt)
        .def("get_states_at", &Profile::getStatesAt)
        .def("get_axes_at", &Profile::getAxesAt)

        .def_static("undefined", &Profile::Undefined)
        .def_static("inertial_pointing", &Profile::InertialPointing)
        .def_static("nadir_pointing", &Profile::NadirPointing)

    ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile") ;

    // Add __path__ attribute for "profile" submodule
    profile.attr("__path__") = "ostk.astrodynamics.flight.profile" ;

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State(profile) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
