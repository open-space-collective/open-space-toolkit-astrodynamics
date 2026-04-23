/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedConstantThrust.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_EffectivityGatedConstantThrust(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::mathematics::object::Vector3d;

    using ostk::astrodynamics::GuidanceLaw;
    using ostk::astrodynamics::guidancelaw::EffectivityGatedConstantThrust;
    using ostk::astrodynamics::guidancelaw::QLaw;

    class_<EffectivityGatedConstantThrust, GuidanceLaw, Shared<EffectivityGatedConstantThrust>>(
        aModule,
        "EffectivityGatedConstantThrust",
        R"doc(
            Constant thrust guidance law gated by QLaw's effectivity thresholds.

            Steering is a constant unit vector held fixed in the theta-R-H (along-track, radial,
            normal) frame. At each step, the held QLaw evaluates direction-aware effectivity for that
            constant direction; if either defined threshold is missed, zero thrust is applied for the
            step. When neither threshold is defined on the held QLaw, the gate short-circuits and
            behavior reduces to constant thrust in theta-R-H.

        )doc"
    )

        .def(
            init<const Vector3d&, const Shared<const QLaw>&>(),
            R"doc(
                Constructor.

                Args:
                    thrust_direction_theta_r_h (np.array): Thrust direction in theta-R-H [theta, R, H]. Normalized internally. Must be non-zero.
                    q_law (QLaw): QLaw used to compute effectivity and supply thresholds. Must be non-null.

            )doc",
            arg("thrust_direction_theta_r_h"),
            arg("q_law")
        )

        .def("__str__", &(shiftToString<EffectivityGatedConstantThrust>))
        .def("__repr__", &(shiftToString<EffectivityGatedConstantThrust>))

        .def(
            "get_thrust_direction_theta_r_h",
            &EffectivityGatedConstantThrust::getThrustDirectionThetaRH,
            R"doc(
                Get the normalized thrust direction in theta-R-H.

                Returns:
                    np.array: The unit thrust direction [theta, R, H].
            )doc"
        )

        .def(
            "access_q_law",
            &EffectivityGatedConstantThrust::accessQLaw,
            R"doc(
                Access the held QLaw.

                Returns:
                    QLaw: The held QLaw.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &EffectivityGatedConstantThrust::calculateThrustAccelerationAt,
            R"doc(
                Compute the thrust acceleration, gated by effectivity thresholds.

                Returns thrust_acceleration * R_thetaRH_to_GCRF * thrust_direction_theta_r_h when both
                effectivity thresholds (where defined on the held QLaw) are met; a zero vector
                otherwise.

                Args:
                    instant (Instant): Instant of computation.
                    position_coordinates (np.array): Position coordinates.
                    velocity_coordinates (np.array): Velocity coordinates.
                    thrust_acceleration (float): Thrust acceleration magnitude.
                    output_frame (Frame): The frame the acceleration is expressed in.

                Returns:
                    np.array: The thrust acceleration.
            )doc",
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame")
        )

        ;
}
