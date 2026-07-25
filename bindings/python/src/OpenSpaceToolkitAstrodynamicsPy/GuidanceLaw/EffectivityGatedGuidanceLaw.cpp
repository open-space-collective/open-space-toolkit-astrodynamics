/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedGuidanceLaw.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_EffectivityGatedGuidanceLaw(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::astrodynamics::GuidanceLaw;
    using ostk::astrodynamics::guidancelaw::EffectivityGatedGuidanceLaw;
    using ostk::astrodynamics::guidancelaw::EffectivityProvider;

    class_<EffectivityGatedGuidanceLaw, GuidanceLaw, Shared<EffectivityGatedGuidanceLaw>>(
        aModule,
        "EffectivityGatedGuidanceLaw",
        R"doc(
            Generic decorator that gates any guidance law with any effectivity provider.

            Per step:
              1. Delegate to the base law to compute a proposed thrust acceleration.
              2. If the base law produced ~zero, propagate that.
              3. Express the proposed direction in theta-R-H and consult the provider's gate.
              4. Pass through the base law's output if the gate passes; otherwise return zero.

            Position/velocity inputs are taken to be GCRF (matching the convention shared with
            QLaw and the legacy ``EffectivityGatedConstantThrust``/``InTrack`` laws).

        )doc"
    )

        .def(
            init<const Shared<const GuidanceLaw>&, const Shared<EffectivityProvider>&>(),
            R"doc(
                Constructor.

                Args:
                    base_law (GuidanceLaw): Base guidance law to gate. Must be non-null.
                    provider (EffectivityProvider): Effectivity provider supplying the gate. Must be non-null.

            )doc",
            arg("base_law"),
            arg("provider"),
            keep_alive<1, 2>(),
            keep_alive<1, 3>()
        )

        .def("__str__", &(shiftToString<EffectivityGatedGuidanceLaw>))
        .def("__repr__", &(shiftToString<EffectivityGatedGuidanceLaw>))

        .def(
            "access_base_law",
            &EffectivityGatedGuidanceLaw::accessBaseLaw,
            R"doc(
                Access the wrapped base guidance law.

                Returns:
                    GuidanceLaw: The base guidance law.
            )doc"
        )

        .def(
            "access_provider",
            &EffectivityGatedGuidanceLaw::accessProvider,
            R"doc(
                Access the held effectivity provider.

                Returns:
                    EffectivityProvider: The held provider.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &EffectivityGatedGuidanceLaw::calculateThrustAccelerationAt,
            R"doc(
                Compute the gated thrust acceleration.

                Returns the base law's output if the provider's gate passes; a zero vector
                otherwise.

                Args:
                    instant (Instant): Instant of computation.
                    position_coordinates (np.array): Position coordinates (GCRF).
                    velocity_coordinates (np.array): Velocity coordinates (GCRF).
                    thrust_acceleration (float): Proposed thrust acceleration magnitude.
                    output_frame (Frame): The frame the acceleration is expressed in.

                Returns:
                    np.array: The (possibly zero) thrust acceleration.
            )doc",
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame")
        )

        ;
}
