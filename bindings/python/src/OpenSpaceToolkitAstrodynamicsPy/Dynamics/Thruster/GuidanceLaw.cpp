/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/GuidanceLaw.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/Thruster/GuidanceLaw/ConstantThrust.cpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::dynamics::thruster::GuidanceLaw;

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
    ) const
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
};

void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster_GuidanceLaw(pybind11::module& aModule)
{
    class_<GuidanceLaw, PyGuidanceLaw, Shared<GuidanceLaw>>(
        aModule,
        "GuidanceLaw",
        R"doc(
            Guidance law base class.

            A guidance law is a mathematical model that computes the acceleration
            based on specific guidance law logic.

            Group:
                guidance-law
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

        ;

    // Create "guidance_law" python submodule
    auto guidance_law = aModule.def_submodule("guidance_law");

    // Add __path__ attribute for "guidance_law" submodule
    guidance_law.attr("__path__") = "ostk.astrodynamics.dynamics.guidance_law";

    // Add objects to "guidance_law" submodule
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster_GuidanceLaw_ConstantThrust(guidance_law);
}
