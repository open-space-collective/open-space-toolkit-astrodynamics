/// Apache License 2.0

#include <nanobind/trampoline.h>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/ConstantThrust.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/HeterogeneousGuidanceLaw.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw/QLaw.cpp>

using namespace nanobind;

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::GuidanceLaw;

// Trampoline class for virtual member functions
class PyGuidanceLaw : public GuidanceLaw
{
   public:
    NB_TRAMPOLINE(GuidanceLaw, 3);

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        NB_OVERRIDE(print, anOutputStream, displayDecorator);
    }

    Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        NB_OVERRIDE_PURE_NAME(
            "calculate_thrust_acceleration_at",
            calculateThrustAccelerationAt,
            anInstant,
            aPositionCoordinates,
            aVelocityCoordinates,
            aThrustAcceleration,
            outputFrameSPtr
        );
    }

    Shared<GuidanceLaw> constructUngatedGuidanceLaw() const override
    {
        NB_OVERRIDE_NAME("construct_ungated_guidance_law", constructUngatedGuidanceLaw, );
    }
};

void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw(nanobind::module_& aModule)
{
    class_<GuidanceLaw, PyGuidanceLaw>(
        aModule,
        "GuidanceLaw",
        R"doc(
            Guidance law base class.

            A guidance law is a mathematical model that computes the acceleration
            based on specific guidance law logic.

        )doc"
    )
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
            "construct_ungated_guidance_law",
            // `self` is taken as a Shared<> so that `shared_from_this()` has an owner under
            // nanobind, which does not hold bound instances in a shared_ptr the way pybind11 did.
            [](const Shared<GuidanceLaw>& aGuidanceLawSPtr) -> Shared<GuidanceLaw>
            {
                return aGuidanceLawSPtr->constructUngatedGuidanceLaw();
            },
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
}
