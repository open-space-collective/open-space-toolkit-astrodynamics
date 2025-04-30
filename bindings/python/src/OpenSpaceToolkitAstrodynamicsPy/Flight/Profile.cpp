/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
    using ostk::core::type::Shared;

    using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::flight::Profile;
    using ostk::astrodynamics::flight::profile::Model;
    using ostk::astrodynamics::Trajectory;
    using ostk::astrodynamics::trajectory::Orbit;
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
        .value("Trajectory", Profile::TargetType::Trajectory, "Trajectory")  // Deprecated in favor of TargetPosition
        .value("TargetPosition", Profile::TargetType::TargetPosition, "Target position")
        .value("TargetVelocity", Profile::TargetType::TargetVelocity, "Target velocity")
        .value("Sun", Profile::TargetType::Sun, "Sun")
        .value("Moon", Profile::TargetType::Moon, "Moon")
        .value("VelocityECI", Profile::TargetType::VelocityECI, "Velocity in ECI")
        .value("VelocityECEF", Profile::TargetType::VelocityECEF, "Velocity in ECEF")
        .value("OrbitalMomentum", Profile::TargetType::OrbitalMomentum, "Orbital momentum")
        .value("OrientationProfile", Profile::TargetType::OrientationProfile, "Orientation profile")

        ;

    class_<Profile::Target, Shared<Profile::Target>>(
        profileClass,
        "Target",
        R"doc(
            The target of the profile.

        )doc"
    )

        .def(
            init<const Profile::TargetType&, const Profile::Axis&, const bool&>(),
            R"doc(
                Constructor.

                Args:
                    type (Profile.TargetType): The target type.
                    axis (Profile.Axis): The axis.
                    anti_direction (bool): True if the direction is flipped, False otherwise. Defaults to False.
            )doc",
            arg("type"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_readonly("type", &Profile::Target::type, "The type of the target.")
        .def_readonly("axis", &Profile::Target::axis, "The axis of the target.")
        .def_readonly(
            "anti_direction", &Profile::Target::antiDirection, "True if the direction is flipped, False otherwise."
        )

        ;

    class_<Profile::TrajectoryTarget, Profile::Target, Shared<Profile::TrajectoryTarget>>(
        profileClass,
        "TrajectoryTarget",
        R"doc(
            The trajectory target.

        )doc"
    )

        .def(
            init<const Trajectory&, const Profile::Axis&, const bool&&>(),
            R"doc(
                Constructor.

                Args:
                    trajectory (Trajectory): The trajectory, required only if the target type is `Trajectory`.
                    axis (Profile.Axis): The axis.
                    anti_direction (bool): True if the direction is flipped, False otherwise. Defaults to False.
            )doc",
            arg("trajectory"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_static(
            "target_position",
            &Profile::TrajectoryTarget::TargetPosition,
            R"doc(
                Create a target, which produces a vector pointing from the observer to the target position.
            )doc",
            arg("trajectory"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_static(
            "target_velocity",
            &Profile::TrajectoryTarget::TargetVelocity,
            R"doc(
                Create a target, which produces a vector pointing along the target velocity.
            )doc",
            arg("trajectory"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_readonly(
            "trajectory",
            &Profile::TrajectoryTarget::trajectory,
            "The trajectory of the target. Used to compute the target position or velocity."
        )

        ;

    class_<Profile::OrientationProfileTarget, Profile::Target, Shared<Profile::OrientationProfileTarget>>(
        profileClass,
        "OrientationProfileTarget",
        R"doc(
            The alignment profile target.

        )doc"
    )

        .def(
            init<const Array<Pair<Instant, Vector3d>>&, const Profile::Axis&, const bool&&>(),
            R"doc(
                Constructor.

                Args:
                    orientation_profile (list[Tuple[Instant, Vector3d]]): The orientation profile.
                    axis (Profile.Axis): The axis.
                    anti_direction (bool): True if the direction is flipped, False otherwise. Defaults to False.
            )doc",
            arg("orientation_profile"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_readonly(
            "orientation_profile",
            &Profile::OrientationProfileTarget::orientationProfile,
            "The orientation profile of the target."
        )

        ;

    class_<Profile::CustomTarget, Profile::Target, Shared<Profile::CustomTarget>>(
        profileClass,
        "CustomTarget",
        R"doc(
            The custom target.

        )doc"
    )

        .def(
            init<const std::function<Vector3d(const State&)>&, const Profile::Axis&, const bool&&>(),
            R"doc(
                Constructor.

                Args:
                    orientation_generator (Callable[np.ndarray, State]]): The orientation generator, accepts a state and returns a size 3 array of directions.
                    axis (Profile.Axis): The axis.
                    anti_direction (bool): True if the direction is flipped, False otherwise. Defaults to False.
            )doc",
            arg("orientation_generator"),
            arg("axis"),
            arg("anti_direction") = false
        )

        .def_readonly(
            "orientation_generator",
            &Profile::CustomTarget::orientationGenerator,
            "The orientation generator of the target."
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
            "access_model",
            &Profile::accessModel,
            return_value_policy::reference_internal,
            R"doc(
                Access the profile model.

                Returns:
                    Model: The profile model.
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
            "local_orbital_frame_pointing",
            &Profile::LocalOrbitalFramePointing,
            R"doc(
                Create a profile aligned with the provided local orbital frame type.

                Args:
                    orbit (Orbit): The orbit.
                    orbital_frame_type (OrbitalFrameType): The type of the orbital frame.

                Returns:
                    Profile: The profile aligned with the local orbital frame.
            )doc",
            arg("orbit"),
            arg("orbital_frame_type")
        )

        .def_static(
            "custom_pointing",
            overload_cast<const Orbit&, const std::function<Quaternion(const State&)>&>(&Profile::CustomPointing),
            R"doc(
                Create a custom pointing profile.

                Args:
                    orbit (Orbit): The orbit.
                    orientation_generator (callable[Quaternion, State]): The orientation generator. Typically used in conjunction with `align_and_constrain`.

                Returns:
                    Profile: The custom pointing profile.

            )doc",
            arg("orbit"),
            arg("orientation_generator")
        )

        .def_static(
            "custom_pointing",
            overload_cast<
                const Orbit&,
                const Shared<const Profile::Target>&,
                const Shared<const Profile::Target>&,
                const Angle&>(&Profile::CustomPointing),
            R"doc(
                Create a custom pointing profile.

                Args:
                    orbit (Orbit): The orbit.
                    alignment_target (Profile.Target): The alignment target.
                    clocking_target (Profile.Target): The clocking target.
                    angular_offset (Angle): The angular offset. Defaults to `Angle.Zero()`.

                Returns:
                    Profile: The custom pointing profile.

            )doc",
            arg("orbit"),
            arg("alignment_target"),
            arg("clocking_target"),
            arg_v("angular_offset", Angle::Zero(), "Angle.Zero()")
        )

        .def_static(
            "align_and_constrain",
            &Profile::AlignAndConstrain,
            R"doc(
                Generate a function that provides a quaternion that aligns to the `alignment_target` and constrains to the `clocking_target` for a given state.

                Args:
                    alignment_target (Profile.Target | Profile.TrajectoryTarget | Profile.OrientationProfileTarget | Profile.CustomTarget): The alignment target.
                    clocking_target (Profile.Target | Profile.TrajectoryTarget | Profile.OrientationProfileTarget | Profile.CustomTarget): The clocking target.
                    angular_offset (Angle): The angular offset. Defaults to `Angle.Zero()`.

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
