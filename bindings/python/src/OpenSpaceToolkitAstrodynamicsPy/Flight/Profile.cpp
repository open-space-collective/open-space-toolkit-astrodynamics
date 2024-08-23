/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::flight::Profile;
    using ostk::astrodynamics::flight::profile::Model;
    using ostk::astrodynamics::Trajectory;
    using ostk::astrodynamics::trajectory::State;

    class_<Profile, Shared<Profile>> profileClass(
        aModule,
        "Profile",
        R"doc(
            Spacecraft Flight Profile.

        )doc"
    );

    enum_<Profile::Axis>(
        profileClass,
        "Axis",
        R"doc(
            The axis of the profile.
        )doc"
    )

        .value("X", Profile::Axis::X, "X axis")
        .value("Y", Profile::Axis::Y, "Y axis")
        .value("Z", Profile::Axis::Z, "Z axis")

        ;

    enum_<Profile::TargetType>(
        profileClass,
        "TargetType",
        R"doc(
            The target type of the profile.
        )doc"
    )

        .value("GeocentricNadir", Profile::TargetType::GeocentricNadir, "Geocentric nadir")
        .value("GeodeticNadir", Profile::TargetType::GeodeticNadir, "Geodetic nadir")
        .value("Trajectory", Profile::TargetType::Trajectory, "Trajectory")
        .value("Sun", Profile::TargetType::Sun, "Sun")
        .value("Moon", Profile::TargetType::Moon, "Moon")
        .value("VelocityECI", Profile::TargetType::VelocityECI, "Velocity in ECI")
        .value("VelocityECEF", Profile::TargetType::VelocityECEF, "Velocity in ECEF")
        .value("OrbitalMomentum", Profile::TargetType::OrbitalMomentum, "Orbital momentum")

        ;

    class_<Profile::Target> profileTargetClass(
        profileClass,
        "Target",
        R"doc(
            The target of the profile.

        )doc"
    );

    profileTargetClass

        .def(
            init<const Profile::TargetType&, const Profile::Axis&, const bool&, const Trajectory&>(),
            R"doc(
                Constructor.

                Args:
                    type (Profile.TargetType): The target type.
                    axis (Profile.Axis): The axis.
                    anti_direction (bool): True if the direction is flipped, False otherwise.
                    trajectory (Trajectory): The trajectory, required only if the target type is `Trajectory`.
            )doc",
            arg("type"),
            arg("axis"),
            arg("anti_direction") = false,
            arg_v("trajectory", Trajectory::Undefined(), "Profile.Trajectory.Undefined()")
        )

        .def_readonly("type", &Profile::Target::type, "The type of the target.")
        .def_readonly("axis", &Profile::Target::axis, "The axis of the target.")
        .def_readonly(
            "anti_direction", &Profile::Target::antiDirection, "True if the direction is flipped, False otherwise."
        )
        .def_readonly(
            "trajectory",
            &Profile::Target::trajectory,
            "The trajectory of the target. Required only if the target type is `Trajectory`."
        )

        ;

    profileClass

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

        .def_static(
            "generate_tracking_profile",
            &Profile::GenerateTrackingProfile,
            R"doc(
                Generate a tracking profile.

                Args:
                    orbit (Orbit): The orbit.
                    orientation_generator (callable[Quaternion, State]): The orientation generator. Typically used in conjunction with `align_and_constrain`.

                Returns:
                    Profile: The tracking profile.

            )doc",
            arg("orbit"),
            arg("orientation_generator")
        )

        .def_static(
            "align_and_constrain",
            &Profile::AlignAndConstrain,
            R"doc(
                Generate a function that provides a quaternion that aligns and constrains for a given state.

                Args:
                    alignment_target (Profile.Target): The alignment target.
                    clocking_target (Profile.Target): The clocking target.
                    angular_offset (Angle): The angular offset.

                Returns:
                    callable[Quaternion, State]: The custom orientation.

            )doc",
            arg("alignment_target"),
            arg("clocking_target"),
            arg_v("angular_offset", Angle::Zero(), "Angle.Zero()")
        )

        ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile");

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(profile);
}
