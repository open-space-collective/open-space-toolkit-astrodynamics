/// Apache License 2.0

#include <pybind11/functional.h>  // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Access_Generator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Map;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::spherical::AER;
    using ostk::physics::Environment;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    using ostk::astrodynamics::Access;
    using ostk::astrodynamics::access::AccessTarget;
    using ostk::astrodynamics::access::Generator;
    using ostk::astrodynamics::Trajectory;
    using ostk::astrodynamics::trajectory::State;

    class_<AccessTarget> accessTargetClass(
        aModule,
        "AccessTarget",
        R"doc(
            Represents the configuration for an Access target, including azimuth, elevation, and range intervals, as well
            as position and LLA (Latitude, Longitude, Altitude).
        )doc"
    );

    enum_<AccessTarget::Type>(accessTargetClass, "Type", R"doc(
        Enumeration of Access Target types.
    )doc")
        .value("Fixed", AccessTarget::Type::Fixed)
        .value("Trajectory", AccessTarget::Type::Trajectory)
        .export_values();

    accessTargetClass
        .def(
            "get_type",
            &AccessTarget::accessType,
            R"doc(
                Get the type of the access target.

                Returns:
                    AccessTarget.Type: The type of the access target.
            )doc"
        )
        .def(
            "get_visibility_criterion",
            &AccessTarget::accessVisibilityCriterion,
            R"doc(
                Get the visibility criterion associated with the access target.

                Returns:
                    VisibilityCriterion: The visibility criterion.
            )doc"
        )
        .def(
            "get_trajectory",
            &AccessTarget::accessTrajectory,
            R"doc(
                Get the trajectory associated with the access target.

                Returns:
                    Trajectory: The trajectory.
            )doc"
        )
        .def(
            "get_position",
            &AccessTarget::getPosition,
            R"doc(
                Get the fixed position associated with the access target.

                Returns:
                    Position: The position.
            )doc"
        )
        .def(
            "get_lla",
            &AccessTarget::getLLA,
            arg("celestial"),
            R"doc(
                Get the latitude, longitude, and altitude (LLA) of the access target.

                Args:
                    celestial (Celestial): The celestial body for the LLA computation.

                Returns:
                    LLA: The latitude, longitude, and altitude.
            )doc"
        )
        .def(
            "compute_r_sez_ecef",
            &AccessTarget::computeR_SEZ_ECEF,
            arg("celestial"),
            R"doc(
                Compute the rotation matrix from ECEF to SEZ frame.

                Args:
                    celestial (Celestial): The celestial body for the rotation computation.

                Returns:
                    numpy.ndarray: The rotation matrix (3x3).
            )doc"
        )
        .def_static(
            "from_lla",
            &AccessTarget::FromLLA,
            arg("visibility_criterion"),
            arg("lla"),
            arg("celestial"),
            R"doc(
                Create an AccessTarget from latitude, longitude, and altitude (LLA).

                Args:
                    visibility_criterion (VisibilityCriterion): The visibility criterion.
                    lla (LLA): The latitude, longitude, and altitude.
                    celestial (Celestial): The celestial body.

                Returns:
                    AccessTarget: The created AccessTarget instance.
            )doc"
        )
        .def_static(
            "from_position",
            &AccessTarget::FromPosition,
            arg("visibility_criterion"),
            arg("position"),
            R"doc(
                Create an AccessTarget from a fixed position.

                Args:
                    visibility_criterion (VisibilityCriterion): The visibility criterion.
                    position (Position): The fixed position.

                Returns:
                    AccessTarget: The created AccessTarget instance.
            )doc"
        )
        .def_static(
            "from_trajectory",
            &AccessTarget::FromTrajectory,
            arg("visibility_criterion"),
            arg("trajectory"),
            R"doc(
                Create an AccessTarget from a trajectory.

                Args:
                    visibility_criterion (VisibilityCriterion): The visibility criterion.
                    trajectory (Trajectory): The trajectory.

                Returns:
                    AccessTarget: The created AccessTarget instance.
            )doc"
        );

    class_<Generator, Shared<Generator>>(
        aModule,
        "Generator",
        R"doc(
            An access generator.

        )doc"
    )

        .def(
            init<
                const Environment&,
                const Duration&,
                const Duration&,
                std::function<bool(const Access&)>&,
                std::function<bool(const State&, const State&)>&>(),
            R"doc(
                Constructor.

                Args:
                    environment (Environment): The environment.
                    step (Duration): The step. Defaults to Duration.minutes(1.0).
                    tolerance (Duration): The tolerance. Defaults to Duration.microseconds(1.0).
                    access_filter (function): The access filter. Defaults to None.
                    state_filter (function): The state filter. Defaults to None.

            )doc",
            arg("environment"),
            arg_v("step", DEFAULT_STEP, "Duration.minutes(1.0)"),
            arg_v("tolerance", DEFAULT_TOLERANCE, "Duration.microseconds(1.0)"),
            arg("access_filter") = none(),
            arg("state_filter") = none()
        )

        .def(
            "is_defined",
            &Generator::isDefined,
            R"doc(
                Check if the generator is defined.

                Returns:
                    bool: True if the generator is defined, False otherwise.

            )doc"
        )

        .def(
            "get_step",
            &Generator::getStep,
            R"doc(
                Get the step.

                Returns:
                    Duration: The step.

            )doc"
        )
        .def(
            "get_tolerance",
            &Generator::getTolerance,
            R"doc(
                Get the tolerance.

                Returns:
                    Duration: The tolerance.

            )doc"
        )
        .def(
            "get_access_filter",
            &Generator::getAccessFilter,
            R"doc(
                Get the access filter.

                Returns:
                    function: The access filter.

            )doc"
        )
        .def(
            "get_state_filter",
            &Generator::getStateFilter,
            R"doc(
                Get the state filter.

                Returns:
                    function: The state filter.

            )doc"
        )

        .def(
            "get_condition_function",
            &Generator::getConditionFunction,
            R"doc(
                Get the condition function.

                Args:
                    access_target (AccessTarget): The access target from which the condition function is being evaluated against.
                    to_trajectory (Trajectory): The trajectory to which the condition function is being evaluated against.

                Returns:
                    function: The condition function.

            )doc",
            arg("access_target"),
            arg("to_trajectory")
        )
        .def(
            "compute_accesses",
            overload_cast<const Interval&, const AccessTarget&, const Trajectory&, const bool&>(
                &Generator::computeAccesses, const_
            ),
            R"doc(
                Compute the accesses.

                Args:
                    interval (Interval): The time interval over which to compute accesses.
                    access_target (AccessTarget): The access target to compute the accesses with.
                    to_trajectory (Trajectory): The trajectory to co compute the accesses with.
                    coarse (bool): True to use coarse mode. Defaults to False. Only available for fixed targets.

                Returns:
                    Accesses: The accesses.

            )doc",
            arg("interval"),
            arg("access_target"),
            arg("to_trajectory"),
            arg("coarse") = false
        )
        .def(
            "compute_accesses",
            overload_cast<const Interval&, const Array<AccessTarget>&, const Trajectory&, const bool&>(
                &Generator::computeAccesses, const_
            ),
            R"doc(
                Compute the accesses.

                Args:
                    interval (Interval): The time interval over which to compute accesses.
                    access_targets (list[AccessTarget]): The access targets to compute the accesses with.
                    to_trajectory (Trajectory): The trajectory to co compute the accesses with.
                    coarse (bool): True to use coarse mode. Defaults to False. Only available for fixed targets.

                Returns:
                    Accesses: The accesses.

            )doc",
            arg("interval"),
            arg("access_targets"),
            arg("to_trajectory"),
            arg("coarse") = false
        )
        .def(
            "set_step",
            &Generator::setStep,
            R"doc(
            Set the step.

            Args:
                step (Duration): The step.

        )doc",
            arg("step")
        )
        .def(
            "set_tolerance",
            &Generator::setTolerance,
            R"doc(
            Set the tolerance.

            Args:
                tolerance (Duration): The tolerance.

        )doc",
            arg("tolerance")
        )
        .def(
            "set_access_filter",
            &Generator::setAccessFilter,
            R"doc(
            Set the access filter.

            Args:
                access_filter (function): The access filter.

        )doc",
            arg("access_filter")
        )
        .def(
            "set_state_filter",
            &Generator::setStateFilter,
            R"doc(
            Set the state filter.

            Args:
                state_filter (function): The state filter.

        )doc",
            arg("state_filter")
        )

        .def_static(
            "undefined",
            &Generator::Undefined,
            R"doc(
                Get an undefined generator.

                Returns:
                    Generator: An undefined generator.
            )doc"
        )

        ;
}
