/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_ConstantThrust(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::String;
    using ostk::core::type::Shared;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
    using ostk::astrodynamics::GuidanceLaw;
    using ostk::astrodynamics::guidancelaw::ConstantThrust;

    {
        class_<ConstantThrust, GuidanceLaw, Shared<ConstantThrust>>(
            aModule,
            "ConstantThrust",
            R"doc(
                Constant Thrust, Constant Direction dynamics.

            )doc"
        )

            .def(
                init<const LocalOrbitalFrameDirection&>(),
                R"doc(
                    Constructor.

                    Args:
                        thrust_direction (LocalOrbitalFrameDirection): The thrust direction.

                )doc",
                arg("thrust_direction")
            )

            .def("__str__", &(shiftToString<ConstantThrust>))
            .def("__repr__", &(shiftToString<ConstantThrust>))

            .def(
                "get_local_thrust_direction",
                &ConstantThrust::getLocalThrustDirection,
                R"doc(
                    Get the local thrust direction.

                    Returns:
                        LocalOrbitalFrameDirection: The local thrust direction.

                )doc"
            )

            .def(
                "calculate_thrust_acceleration_at",
                &ConstantThrust::calculateThrustAccelerationAt,
                R"doc(
                    Compute the acceleration due to constant thrust.

                    Args:
                        instant (Instant): The instant of the state vector.
                        position_coordinates (numpy.ndarray): The position coordinates.
                        velocity_coordinates (numpy.ndarray): The velocity coordinates.
                        thrust_acceleration (float): The thrust acceleration magnitude.
                        output_frame (Frame): The frame the acceleration will be expressed in.

                    Returns:
                        numpy.ndarray: The contribution of the constant thrust to the state vector.

                )doc",
                arg("instant"),
                arg("position_coordinates"),
                arg("velocity_coordinates"),
                arg("thrust_acceleration"),
                arg("output_frame")
            )

            .def_static(
                "intrack",
                &ConstantThrust::Intrack,
                R"doc(
                    Create a constant thrust in the in-track direction.

                    Args:
                        satellite_system (SatelliteSystem): The satellite system.
                        velocity_direction (bool, optional): If True, the thrust is applied in the velocity direction. Otherwise, it is applied in the opposite direction.
                        frame (Frame, optional): The reference frame.

                    Returns:
                        ConstantThrust: The constant thrust.
                )doc",
                arg("velocity_direction") = true
            )

            ;
    }
}
