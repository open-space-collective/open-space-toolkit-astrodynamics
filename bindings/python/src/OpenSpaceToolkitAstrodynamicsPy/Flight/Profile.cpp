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

    class_<Profile>(
        aModule,
        "Profile",
        R"doc(
            Spacecraft Flight Profile.

        )doc"
    )

        .def(
            init<const Model&>(),
            R"doc(
                Constructor.

                Args:
                    model (Model): The profile model.
            )doc",
            arg("model")
        )

        .def("__str__", &(shiftToString<Profile>))
        .def("__repr__", &(shiftToString<Profile>))

        .def(
            "is_defined",
            &Profile::isDefined,
            R"doc(
                Check if the profile is defined.

                Returns:
                    bool: True if the profile is defined, False otherwise.
            )doc"
        )

        .def(
            "get_state_at",
            &Profile::getStateAt,
            R"doc(
                Get the state of the profile at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state of the profile at the given instant.
            )doc",
            arg("instant")
        )

        .def(
            "get_states_at",
            &Profile::getStatesAt,
            R"doc(
                Get the states of the profile at given instants.

                Args:
                    instants (list): The instants.

                Returns:
                    list: The states of the profile at the given instants.
            )doc",
            arg("instants")
        )

        .def(
            "get_axes_at",
            &Profile::getAxesAt,
            R"doc(
                Get the axes of the profile at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    Frame: The axes of the profile at the given instant.
            )doc",
            arg("instant")
        )

        .def(
            "get_body_frame",
            &Profile::getBodyFrame,
            R"doc(
                Get the body frame of the profile.

                Args:
                    frame_name (str): The name of the frame.

                Returns:
                    Frame: The body frame of the profile.
            )doc",
            arg("frame_name")
        )

        .def_static(
            "undefined",
            &Profile::Undefined,
            R"doc(
                Create an undefined profile.

                Returns:
                    Profile: The undefined profile.
            )doc"
        )

        .def_static(
            "inertial_pointing",
            &Profile::InertialPointing,
            R"doc(
                Create an inertial pointing profile.

                Args:
                    trajectory (Trajectory): The trajectory.
                    quaternion (Quaternion): The quaternion.

                Returns:
                    Profile: The inertial pointing profile.
            )doc",
            arg("trajectory"),
            arg("quaternion")
        )

        .def_static(
            "nadir_pointing",
            &Profile::NadirPointing,
            R"doc(
                Create a nadir pointing profile.

                Args:
                    orbit (Orbit): The orbit.
                    orbital_frame_type (OrbitalFrameType): The type of the orbital frame.

                Returns:
                    Profile: The nadir pointing profile.
            )doc",
            arg("orbit"),
            arg("orbital_frame_type")
        )

        ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile");

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State(profile);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(profile);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Models(profile);
}
