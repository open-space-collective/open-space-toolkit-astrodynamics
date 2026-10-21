/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/ConstantThrust.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/HeterogeneousGuidanceLaw.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/QLaw.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/ROETargeting.cpp>

using namespace pybind11;

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::GuidanceLaw;

// Trampoline class for virtual member functions
class PyGuidanceLaw : public GuidanceLaw
{
   public:
    using GuidanceLaw::GuidanceLaw;

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        PYBIND11_OVERRIDE(void, GuidanceLaw, print, anOutputStream, displayDecorator);
    }

    Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Vector3d,
            GuidanceLaw,
            "calculate_thrust_acceleration_at",
            calculateThrustAccelerationAt,
            anInstant,
            aPositionCoordinates,
            aVelocityCoordinates,
            aThrustAcceleration,
            outputFrameSPtr
        );
    }

    std::optional<CommittedManeuver> commitManeuverAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Shared<const Frame>& aFrameSPtr
    ) const override
    {
        PYBIND11_OVERRIDE_NAME(
            std::optional<CommittedManeuver>,
            GuidanceLaw,
            "commit_maneuver_at",
            commitManeuverAt,
            anInstant,
            aPositionCoordinates,
            aVelocityCoordinates,
            aFrameSPtr
        );
    }

    Shared<GuidanceLaw> constructUngatedGuidanceLaw() const override
    {
        PYBIND11_OVERRIDE_NAME(
            Shared<GuidanceLaw>, GuidanceLaw, "construct_ungated_guidance_law", constructUngatedGuidanceLaw,
        );
    }
};

void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw(pybind11::module& aModule)
{
    class_<GuidanceLaw, PyGuidanceLaw, Shared<GuidanceLaw>> guidanceLaw(
        aModule,
        "GuidanceLaw",
        R"doc(
            Guidance law base class.

            A guidance law is a mathematical model that computes the acceleration
            based on specific guidance law logic.

        )doc"
    );

    class_<GuidanceLaw::CommittedManeuver>(
        guidanceLaw,
        "CommittedManeuver",
        R"doc(
            A maneuver that a guidance law commits to when it starts.
        )doc"
    )
        .def(
            init<const Shared<const GuidanceLaw>&, const Duration&>(),
            R"doc(
                Constructor.

                Args:
                    guidance_law (GuidanceLaw): The guidance law to follow during the maneuver.
                    duration (Duration): The maneuver duration.
            )doc",
            arg("guidance_law"),
            arg("duration")
        )
        .def_readonly(
            "guidance_law",
            &GuidanceLaw::CommittedManeuver::guidanceLaw,
            R"doc(
                The guidance law to follow during the maneuver.

                Type:
                    GuidanceLaw
            )doc"
        )
        .def_readonly(
            "duration",
            &GuidanceLaw::CommittedManeuver::duration,
            R"doc(
                The maneuver duration.

                Type:
                    Duration
            )doc"
        );

    guidanceLaw

        .def(
            init<const String&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the guidance law.

            )doc",
            arg("name")
        )

        .def(
            "get_name",
            &GuidanceLaw::getName,
            R"doc(
                Get the name of the guidance law.

                Returns:
                    str: The name of the guidance law.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &GuidanceLaw::calculateThrustAccelerationAt,
            R"doc(
                Compute the acceleration.

                Args:
                    instant (Instant): Instant of computation.
                    position_coordinates (np.array): Position coordinates.
                    velocity_coordinates (np.array): Velocity coordinates.
                    thrust_acceleration (float): Thrust acceleration magnitude.
                    output_frame (Frame): The frame the acceleration is expressed in.

                Returns:
                    np.array: The acceleration.
            )doc",
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame")
        )

        .def(
            "commit_maneuver_at",
            &GuidanceLaw::commitManeuverAt,
            R"doc(
                Commit to a maneuver starting at the provided coordinates.

                Guidance laws that plan discrete maneuvers (e.g. with model predictive control) decide at every
                evaluation whether a maneuver is worth starting. Re-evaluated along a maneuver, they would change their
                decision, as the maneuver itself erodes the reasons to perform it. When such a guidance law starts a
                maneuver, a maneuvering Segment asks it for the guidance law to follow during the whole maneuver and
                for the maneuver duration, instead of re-evaluating it along the maneuver.

                The default implementation does not commit.

                Args:
                    instant (Instant): The start instant of the maneuver.
                    position_coordinates (np.array): The position coordinates at the start of the maneuver.
                    velocity_coordinates (np.array): The velocity coordinates at the start of the maneuver.
                    frame (Frame): The frame of the coordinates.

                Returns:
                    GuidanceLaw.CommittedManeuver | None: The committed maneuver, if any.
            )doc",
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("frame")
        )

        .def(
            "construct_ungated_guidance_law",
            &GuidanceLaw::constructUngatedGuidanceLaw,
            R"doc(
                Construct an ungated version of the instance.

                An 'ungated' guidance law is one that always returns a non-zero acceleration vector.

                Returns:
                    GuidanceLaw: The ungated guidance law instance.
            )doc"
        )

        ;

    // Create "guidance_law" python submodule
    auto guidance_law = aModule.def_submodule("guidance_law");

    // Add objects to "guidance_law" submodule
    OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_ConstantThrust(guidance_law);
    OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_QLaw(guidance_law);
    OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_HeterogeneousGuidanceLaw(guidance_law);
    OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_ROETargeting(guidance_law);
}
