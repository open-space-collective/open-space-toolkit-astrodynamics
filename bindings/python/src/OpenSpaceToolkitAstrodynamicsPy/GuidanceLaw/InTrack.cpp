/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/InTrack.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_InTrack(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::astrodynamics::GuidanceLaw;
    using ostk::astrodynamics::guidancelaw::EffectivityProvider;
    using ostk::astrodynamics::guidancelaw::InTrack;
    using ostk::astrodynamics::guidancelaw::QLaw;

    class_<InTrack, GuidanceLaw, Shared<InTrack>>(
        aModule,
        "InTrack",
        R"doc(
            In-track guidance law gated by an effectivity provider.

            Steering is restricted to ±along-track (prograde / retrograde) in the theta-R-H
            (along-track, radial, normal) frame. At each step the held provider scores both
            [+1, 0, 0] and [-1, 0, 0]; the candidate with the higher score is selected and gated
            against the same provider. If the gate fails for the chosen direction, zero thrust is
            applied for that step.

        )doc"
    )

        .def(
            init<const Shared<EffectivityProvider>&>(),
            R"doc(
                Constructor (provider-based).

                Args:
                    provider (EffectivityProvider): Effectivity provider supplying both the
                        directional score (used to pick prograde vs. retrograde) and the gate.
                        Must be non-null and must produce a defined score for the candidate
                        directions.

            )doc",
            arg("provider"),
            keep_alive<1, 2>()
        )

        .def(
            init<const Shared<const QLaw>&>(),
            R"doc(
                Constructor (QLaw-based, kept for backwards compatibility).

                Equivalent to constructing an ``effectivity_provider.QLaw`` from ``q_law`` and
                forwarding to the provider-based constructor. New code should prefer the
                provider-based form.

                Args:
                    q_law (QLaw): QLaw used to compute effectivity and supply thresholds. Must be non-null.

            )doc",
            arg("q_law")
        )

        .def("__str__", &(shiftToString<InTrack>))
        .def("__repr__", &(shiftToString<InTrack>))

        .def(
            "access_provider",
            &InTrack::accessProvider,
            R"doc(
                Access the held effectivity provider.

                Returns:
                    EffectivityProvider: The held provider.
            )doc"
        )

        .def(
            "access_q_law",
            &InTrack::accessQLaw,
            R"doc(
                Access the held QLaw, if the provider wraps one (legacy accessor).

                Returns the QLaw embedded in an ``effectivity_provider.QLaw`` when present;
                otherwise None. New code should prefer ``access_provider``.

                Returns:
                    QLaw: The held QLaw, or None if the provider is not a QLaw provider.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &InTrack::calculateThrustAccelerationAt,
            R"doc(
                Compute the thrust acceleration, auto-selecting prograde or retrograde and gating
                via the held provider.

                Returns thrust_acceleration * R_thetaRH_to_GCRF * chosen_direction when the gate
                passes; a zero vector otherwise.

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
