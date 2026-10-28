/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
    using ostk::core::type::Shared;

    using ostk::mathematics::curvefitting::Interpolator;
    using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::environment::object::Celestial;

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

    enum_<Profile::TargetType>(
        profileClass,
        "TargetType",
        R"doc(
            The type of target to point towards.
        )doc"
    )

        .value(
            "GeocentricNadir",
            Profile::TargetType::GeocentricNadir,
            "Negative of the position vector of the satellite in the ECI frame"
        )
        .value(
            "GeodeticNadir",
            Profile::TargetType::GeodeticNadir,
            "Negative of the geodetic normal of the satellite in the ECI frame"
        )
        .value("VelocityECI", Profile::TargetType::VelocityECI, "The velocity vector of the satellite in the ECI frame")
        .value(
            "VelocityECEF", Profile::TargetType::VelocityECEF, "The velocity vector of the satellite in the ECEF frame"
        )
        .value(
            "OrbitalMomentum",
            Profile::TargetType::OrbitalMomentum,
            "The orbital momentum vector of the satellite in the ECI frame"
        )
        .value("Sun", Profile::TargetType::Sun, "The position of the Sun")
        .value("Moon", Profile::TargetType::Moon, "The position of the Moon")
        .value(
            "TrajectoryPosition",
            Profile::TargetType::TrajectoryPosition,
            "Points towards the provided trajectory's position"
        )
        .value(
            "TrajectoryVelocity",
            Profile::TargetType::TrajectoryVelocity,
            "Points along the provided trajectory's velocity vector"
        )
        .value(
            "TrajectorySlidingGroundVelocity",
            Profile::TargetType::TrajectorySlidingGroundVelocity,
            "Points along the provided trajectory's ground velocity vector (aka the scan direction of the point "
            "sliding across the ground)"
        )
        .value(
            "CustomProfileECI",
            Profile::TargetType::CustomProfileECI,
            "Pointing pre-defined via a profile of vectors in the ECI frame"
        )
        .value(
            "CustomProfileGeneratorECI",
            Profile::TargetType::CustomProfileGeneratorECI,
            "Pointing generated at each timestep by a function that returns a vector in the ECI frame"
        )

        ;

    class_<Profile::Pointing, Shared<Profile::Pointing>>(
        profileClass,
        "Pointing",
        R"doc(
            Represents pointing of one axis of your spacecraft towards a target.

        )doc"
    )

        .def(
            init<const Profile::TargetType&, const Vector3d&>(),
            R"doc(
                Constructor.

                Args:
                    type (Profile.TargetType): The type of the target.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for the Z axis).
            )doc",
            arg("type"),
            arg("direction_in_body_frame")
        )

        .def_readonly("type", &Profile::Pointing::type, "The type of the target.")
        .def_readonly("direction", &Profile::Pointing::direction, "The direction (unit vector) of the target.")

        ;

    class_<Profile::TrajectoryPointing, Profile::Pointing, Shared<Profile::TrajectoryPointing>>(
        profileClass,
        "TrajectoryPointing",
        R"doc(
            Represents pointing of one axis of your spacecraft towards a trajectory.

        )doc"
    )

        .def_static(
            "target_position",
            &Profile::TrajectoryPointing::TargetPosition,
            R"doc(
                Create a trajectory pointing, pointing towards the provided trajectory's position.

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target.

                Returns:
                    Profile.TrajectoryPointing: The trajectory pointing.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_static(
            "target_velocity",
            &Profile::TrajectoryPointing::TargetVelocity,
            R"doc(
                Create a trajectory pointing, pointing towards the provided trajectory's velocity vector.
                When choosing this as a clocking target, the resulting profile will not compensate for the rotation of the referenced celestial body (yaw compensation).

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target.

                Returns:
                    Profile.TrajectoryPointing: The trajectory pointing.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_static(
            "target_sliding_ground_velocity",
            &Profile::TrajectoryPointing::TargetSlidingGroundVelocity,
            R"doc(
                Create a trajectory pointing, pointing along the provided trajectory's ground velocity vector (aka the scan direction of the point sliding across the ground).
                When choosing this as a clocking target, the resulting profile will compensate for the rotation of the referenced celestial body (yaw compensation).

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target.

                Returns:
                    Profile.TrajectoryPointing: The trajectory pointing.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_readonly("trajectory", &Profile::TrajectoryPointing::trajectory, "The trajectory to point towards.")

        ;

    class_<Profile::CustomProfilePointing, Profile::Pointing, Shared<Profile::CustomProfilePointing>>(
        profileClass,
        "CustomProfilePointing",
        R"doc(
            Represents pointing of one axis of your spacecraft towards a custom pointing profile in the ECI frame.
            The profile is provided up front (must be known beforehand).

        )doc"
    )

        .def(
            init<const Array<Pair<Instant, Vector3d>>&, const Vector3d&, const Interpolator::Type&>(),
            R"doc(
                Constructor.

                Args:
                    pointing_profile_eci (list[Tuple[Instant, Vector3d]]): The profile of pointing vectors in the ECI frame.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target.
                    interpolator_type (Interpolator.Type, optional): The type of interpolator to use. Defaults to Barycentric Rational.
            )doc",
            arg("pointing_profile_eci"),
            arg("direction_in_body_frame"),
            arg_v("interpolator_type", Interpolator::Type::BarycentricRational, "Interpolator.Type.BarycentricRational")
        )

        .def_readonly(
            "pointing_profile_eci",
            &Profile::CustomProfilePointing::pointingProfileECI,
            "The profile of pointing vectors in the ECI frame."
        )

        ;

    class_<Profile::CustomProfileGeneratorPointing, Profile::Pointing, Shared<Profile::CustomProfileGeneratorPointing>>(
        profileClass,
        "CustomProfileGeneratorPointing",
        R"doc(
            Represents pointing of one axis of your spacecraft towards a custom pointing profile generator.
            The pointing vector is generated at each timestep by a function that returns a vector in the ECI frame (does not need to be known beforehand).

        )doc"
    )

        .def(
            init<const std::function<Vector3d(const State&)>&, const Vector3d&>(),
            R"doc(
                Constructor.

                Args:
                    pointing_profile_generator_eci (Callable[[State], np.ndarray]): The function that generates the pointing vector in the ECI frame from the state at a given instant.
                    direction_in_body_frame (Vector3d): The (unit) vector in your spacecraft's body frame to point relative to the target.
            )doc",
            arg("pointing_profile_generator_eci"),
            arg("direction_in_body_frame")
        )

        .def_readonly(
            "pointing_profile_generator_eci",
            &Profile::CustomProfileGeneratorPointing::pointingProfileGeneratorECI,
            "The function that generates the pointing vector in the ECI frame."
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
                Get the spacecraft state at a given instant.

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
                Get the spacecraft states at given instants.

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
                Get the spacecraft axes at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    Frame: The axes of the profile at the given instant.
            )doc",
            arg("instant")
        )

        .def(
            "construct_body_frame",
            &Profile::constructBodyFrame,
            R"doc(
                Construct the body frame of the profile.

                Args:
                    frame_name (str): The name of the frame.
                    overwrite (bool): If True, destruct existing frame with same name. Defaults to False.

                Returns:
                    Frame: The body frame of the profile.
            )doc",
            arg("frame_name"),
            arg("overwrite") = false
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
            "constant_inertial_pointing",
            &Profile::ConstantInertialPointing,
            R"doc(
                Create a profile with constant inertial pointing.

                Args:
                    trajectory (Trajectory): The trajectory.
                    quaternion (Quaternion): The pointing in GCRF.

                Returns:
                    Profile: The constant inertial pointing profile.
            )doc",
            arg("trajectory"),
            arg("quaternion")
        )

        .def_static(
            "constant_local_orbital_frame_pointing",
            &Profile::ConstantLocalOrbitalFramePointing,
            R"doc(
                Create a profile with a constant local orbital frame pointing.

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
            "constrained_pointing",
            &Profile::ConstrainedPointing,
            R"doc(
                Create a profile with constrained (alignment vector + clocking vector) pointing.

                Args:
                    orbit (Orbit): The orbit.
                    alignment_pointing (Profile.Pointing): The alignment pointing.
                    clocking_pointing (Profile.Pointing): The clocking pointing.
                    angular_clocking_offset (Angle): The angular offset applied to the clocking pointing vector. Defaults to `Angle.Zero()`.

                Returns:
                    Profile: The constrained pointing profile.

            )doc",
            arg("orbit"),
            arg("alignment_pointing"),
            arg("clocking_pointing"),
            arg_v("angular_clocking_offset", Angle::Zero(), "Angle.Zero()")
        )

        .def_static(
            "custom_pointing",
            &Profile::CustomPointing,
            R"doc(
                Create a profile with fully custom pointing.

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
            "align_and_constrain",
            &Profile::AlignAndConstrain,
            R"doc(
                Generate a function that returns a quaternion constrained by alignment and clocking pointing vectors for each given state.

                Args:
                    alignment_pointing (Profile.Pointing | Profile.TrajectoryPointing | Profile.CustomProfilePointing | Profile.CustomProfileGeneratorPointing): The alignment pointing.
                    clocking_pointing (Profile.Pointing | Profile.TrajectoryPointing | Profile.CustomProfilePointing | Profile.CustomProfileGeneratorPointing): The clocking pointing.
                    celestial (Celestial): The celestial object. Its body frame will be used for geodetic nadir and sliding ground velocity calculations.
                    angular_clocking_offset (Angle): The angular offset applied to the clocking pointing vector. Defaults to `Angle.Zero()`.

                Returns:
                    callable[Quaternion, State]: The custom orientation.

            )doc",
            arg("alignment_pointing"),
            arg("clocking_pointing"),
            arg("celestial"),
            arg_v("angular_clocking_offset", Angle::Zero(), "Angle.Zero()")
        )

        ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile");

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(profile);
}
