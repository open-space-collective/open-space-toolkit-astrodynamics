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
            "Tabulated",
            Profile::TargetType::Tabulated,
            "Pointing pre-defined via a profile of vectors in the ECI frame that can be interpolated"
        )
        .value(
            "Generator",
            Profile::TargetType::Generator,
            "Pointing generated at each timestep by a function that returns a vector in the ECI frame"
        )

        ;

    class_<Profile::PointingConstraint, Shared<Profile::PointingConstraint>>(
        profileClass,
        "PointingConstraint",
        R"doc(
            Represents a pointing constraint of one axis of the spacecraft towards a target.

        )doc"
    )

        .def(
            init<const Profile::TargetType&, const Vector3d&>(),
            R"doc(
                Constructor.

                Args:
                    type (Profile.TargetType): The type of the target.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).
            )doc",
            arg("type"),
            arg("direction_in_body_frame")
        )

        .def_readonly("type", &Profile::PointingConstraint::type, "The type of the target.")
        .def_readonly(
            "direction", &Profile::PointingConstraint::direction, "The direction (unit vector) of the target."
        )

        ;

    class_<
        Profile::TrajectoryPointingConstraint,
        Profile::PointingConstraint,
        Shared<Profile::TrajectoryPointingConstraint>>(
        profileClass,
        "TrajectoryPointingConstraint",
        R"doc(
            Represents a pointing constraint of one axis of the spacecraft towards a trajectory.

        )doc"
    )

        .def_static(
            "target_position",
            &Profile::TrajectoryPointingConstraint::TargetPosition,
            R"doc(
                Create a trajectory pointing constraint, pointing towards the provided trajectory's position.

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).

                Returns:
                    Profile.TrajectoryPointingConstraint: The trajectory pointing constraint.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_static(
            "target_velocity",
            &Profile::TrajectoryPointingConstraint::TargetVelocity,
            R"doc(
                Create a trajectory pointing constraint, pointing towards the provided trajectory's velocity vector.
                When choosing this as a clocking target, the resulting profile will not compensate for the rotation of the referenced celestial body (yaw compensation).

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).

                Returns:
                    Profile.TrajectoryPointingConstraint: The trajectory pointing constraint.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_static(
            "target_sliding_ground_velocity",
            &Profile::TrajectoryPointingConstraint::TargetSlidingGroundVelocity,
            R"doc(
                Create a trajectory pointing constraint, pointing along the provided trajectory's ground velocity vector (aka the scan direction of the point sliding across the ground).
                When choosing this as a clocking target, the resulting profile will compensate for the rotation of the referenced celestial body (yaw compensation).

                Args:
                    trajectory (Trajectory): The trajectory to point towards.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).

                Returns:
                    Profile.TrajectoryPointingConstraint: The trajectory pointing constraint.
            )doc",
            arg("trajectory"),
            arg("direction_in_body_frame")
        )

        .def_readonly(
            "trajectory", &Profile::TrajectoryPointingConstraint::trajectory, "The trajectory to point towards."
        )

        ;

    class_<
        Profile::TabulatedPointingConstraint,
        Profile::PointingConstraint,
        Shared<Profile::TabulatedPointingConstraint>>(
        profileClass,
        "TabulatedPointingConstraint",
        R"doc(
            Represents a pointing constraint of one axis of the spacecraft towards a user defined, tabulated pointing profile in the ECI frame.
            Unlike the pre-defined target types, the target is fully user defined: the profile is provided up front (must be known beforehand) and interpolated at each instant.

        )doc"
    )

        .def(
            init<const Array<Pair<Instant, Vector3d>>&, const Vector3d&, const Interpolator::Type&>(),
            R"doc(
                Constructor.

                Args:
                    pointing_profile_eci (list[Tuple[Instant, Vector3d]]): The profile of pointing vectors in the ECI frame.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).
                    interpolator_type (Interpolator.Type, optional): The type of interpolator to use. Defaults to Barycentric Rational.
            )doc",
            arg("pointing_profile_eci"),
            arg("direction_in_body_frame"),
            arg_v("interpolator_type", Interpolator::Type::BarycentricRational, "Interpolator.Type.BarycentricRational")
        )

        .def_readonly(
            "pointing_profile_eci",
            &Profile::TabulatedPointingConstraint::pointingProfileECI,
            "The profile of pointing vectors in the ECI frame."
        )

        ;

    class_<
        Profile::PointingConstraintGenerator,
        Profile::PointingConstraint,
        Shared<Profile::PointingConstraintGenerator>>(
        profileClass,
        "PointingConstraintGenerator",
        R"doc(
            Represents a pointing constraint of one axis of the spacecraft towards a user defined pointing vector generator.
            Unlike the pre-defined target types, the target is fully user defined: the pointing vector is generated at each timestep by a function that returns a vector in the ECI frame (does not need to be known beforehand).

        )doc"
    )

        .def(
            init<const std::function<Vector3d(const State&)>&, const Vector3d&>(),
            R"doc(
                Constructor.

                Args:
                    pointing_profile_generator_eci (Callable[[State], np.ndarray]): The function that generates the pointing vector in the ECI frame from the state at a given instant.
                    direction_in_body_frame (Vector3d): The (unit) vector in the spacecraft's body frame to point relative to the target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69] for an arbitrary axis).
            )doc",
            arg("pointing_profile_generator_eci"),
            arg("direction_in_body_frame")
        )

        .def_readonly(
            "pointing_profile_generator_eci",
            &Profile::PointingConstraintGenerator::pointingProfileGeneratorECI,
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
                Create a profile with constrained pointing, where the spacecraft is aligned with the alignment pointing constraint and clocked with the clocking pointing constraint.

                This solves the "align and clock" (two-vector) attitude problem at each instant of the orbit. Fully defining the spacecraft attitude requires fixing 3 rotational degrees of freedom, which is done using two constraints of decreasing priority:

                - Alignment (primary constraint): the body frame direction of the alignment pointing constraint is pointed exactly towards its target (e.g. the payload boresight towards geodetic nadir). This fixes 2 degrees of freedom, leaving the spacecraft free to rotate about its alignment direction.
                - Clocking (secondary constraint): the remaining degree of freedom (the rotation about the alignment direction) is fixed by rotating the body frame direction of the clocking pointing constraint as close as possible to its target (e.g. a body axis along the ground velocity, for yaw steering).
                The celestial object of the orbit is used for the target types that depend on the rotation or shape of the central body (geodetic nadir, ECEF velocity and sliding ground velocity). See `align_and_clock` for details on how the orientation is computed.

                Args:
                    orbit (Orbit): The orbit.
                    alignment_pointing_constraint (Profile.PointingConstraint): The alignment pointing constraint (primary constraint, met exactly).
                    clocking_pointing_constraint (Profile.PointingConstraint): The clocking pointing constraint (secondary constraint, met as closely as possible).
                    angular_clocking_offset (Angle): The angular offset applied to the clocking target, as a right-handed rotation about the alignment target. Defaults to `Angle.Zero()`.

                Returns:
                    Profile: The constrained pointing profile.

            )doc",
            arg("orbit"),
            arg("alignment_pointing_constraint"),
            arg("clocking_pointing_constraint"),
            arg_v("angular_clocking_offset", Angle::Zero(), "Angle.Zero()")
        )

        .def_static(
            "custom_pointing",
            &Profile::CustomPointing,
            R"doc(
                Create a profile with fully custom pointing.

                Args:
                    orbit (Orbit): The orbit.
                    orientation_generator (callable[Quaternion, State]): The orientation generator. Typically used in conjunction with `align_and_clock`.

                Returns:
                    Profile: The custom pointing profile.

            )doc",
            arg("orbit"),
            arg("orientation_generator")
        )

        .def_static(
            "align_and_clock",
            &Profile::AlignAndClock,
            R"doc(
                Generate a function that returns a quaternion constrained by alignment and clocking vectors for each given state.

                The generated function solves the "align and clock" (two-vector) attitude problem: the spacecraft alignment body frame direction is pointed exactly towards the alignment target (primary constraint), and the spacecraft is then rotated about this direction so that its clocking body frame direction gets as close as possible to the clocking target (secondary constraint).

                For each state, with all target directions expressed in GCRF:

                1. The alignment target direction is computed from the alignment pointing constraint.
                2. The clocking target direction is computed from the clocking pointing constraint, then projected onto the plane perpendicular to the alignment target direction, and rotated about the alignment target direction by the angular clocking offset.
                3. The clocking body frame direction is projected onto the plane perpendicular to the alignment body frame direction.
                4. The returned quaternion is the rotation from GCRF to the spacecraft body frame that maps the alignment target direction onto the alignment body frame direction, and the projected clocking target direction onto the projected clocking body frame direction.

                The resulting attitude is undefined if the alignment and clocking target directions are colinear at a given state, as there is no unique rotation about the alignment target direction in that case.

                This function is typically used in conjunction with `custom_pointing`, or directly through `constrained_pointing`.

                Args:
                    celestial (Celestial): The celestial object. Its body frame will be used for geodetic nadir, ECEF velocity and sliding ground velocity calculations.
                    alignment_pointing_constraint (Profile.PointingConstraint | Profile.TrajectoryPointingConstraint | Profile.TabulatedPointingConstraint | Profile.PointingConstraintGenerator): The alignment pointing constraint (primary constraint, met exactly).
                    clocking_pointing_constraint (Profile.PointingConstraint | Profile.TrajectoryPointingConstraint | Profile.TabulatedPointingConstraint | Profile.PointingConstraintGenerator): The clocking pointing constraint (secondary constraint, met as closely as possible).
                    angular_clocking_offset (Angle): The angular offset applied to the clocking target, as a right-handed rotation about the alignment target. Defaults to `Angle.Zero()`.

                Returns:
                    callable[Quaternion, State]: The orientation (rotation from GCRF to the spacecraft body frame) for a given state.

                Raises:
                    RuntimeError: If the alignment and clocking pointing constraints have the same target type (except for trajectory position and trajectory velocity targets, which can point to different trajectories).
                    RuntimeError: If the alignment and clocking body frame directions are colinear.
                    RuntimeError: If the alignment and clocking targets are geocentric nadir and geodetic nadir.

            )doc",
            arg("celestial"),
            arg("alignment_pointing_constraint"),
            arg("clocking_pointing_constraint"),
            arg_v("angular_clocking_offset", Angle::Zero(), "Angle.Zero()")
        )

        ;

    // Create "profile" python submodule
    auto profile = aModule.def_submodule("profile");

    // Add objects to "profile" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(profile);
}
