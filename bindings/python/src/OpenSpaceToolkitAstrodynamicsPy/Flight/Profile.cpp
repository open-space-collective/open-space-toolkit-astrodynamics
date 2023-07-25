/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Models.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/State.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::Shared;

    using ostk::physics::coord::Frame;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::Model;
    using ostk::astro::flight::profile::State;

    class_<Profile>(aModule, "Profile")

        .def(init<const Model&>(), arg("model"))

        .def("__str__", &(shiftToString<Profile>))
        .def("__repr__", &(shiftToString<Profile>))

        .def("is_defined", &Profile::isDefined)

        .def("get_state_at", &Profile::getStateAt, arg("instant"))
        .def("get_states_at", &Profile::getStatesAt, arg("instants"))
        .def("get_axes_at", &Profile::getAxesAt, arg("instant"))
        .def("get_body_frame", &Profile::getBodyFrame, arg("frame_name"))

        .def_static("undefined", &Profile::Undefined)
        .def_static("inertial_pointing", &Profile::InertialPointing, arg("trajectory"), arg("quaternion"))
        .def_static("nadir_pointing", &Profile::NadirPointing, arg("orbit"), arg("orbital_frame_type"))

        ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile");

    // Add __path__ attribute for "profile" submodule
    profile.attr("__path__") = "ostk.astrodynamics.flight.profile";

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State(profile);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(profile);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Models(profile);
}
