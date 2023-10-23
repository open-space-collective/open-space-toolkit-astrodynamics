/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/ConstantThrust.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster_ConstantThrust(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::String;
    using ostk::core::types::Shared;

    using ostk::physics::coord::Frame;

    using ostk::astro::Dynamics;
    using ostk::astro::trajectory::LocalOrbitalFrameDirection;
    using ostk::astro::dynamics::Thruster;
    using ostk::astro::dynamics::thruster::ConstantThrust;
    using ostk::astro::flight::system::SatelliteSystem;

    {
        class_<ConstantThrust, Thruster, Shared<ConstantThrust>>(
            aModule,
            "ConstantThrust",
            R"doc(
                Constant Thrust, Constant Direction dynamics.

                Group:
                    Thruster
            )doc"
        )
            .def(
                init<const SatelliteSystem&, const LocalOrbitalFrameDirection&, const String&>(),
                R"doc(
                    Constructor.

                    Args:
                        satellite_system (SatelliteSystem): The satellite system.
                        thrust_direction (LocalOrbitalFrameDirection): The thrust direction.
                        name (str, optional): The name of the thruster.

                )doc",
                arg("satellite_system"),
                arg("thrust_direction"),
                arg("name") = String::Empty()
            )

            .def(
                "is_defined",
                &ConstantThrust::isDefined,
                R"doc(
                    Check if the constant thrust is defined.

                    Returns:
                        bool: True if the constant thrust is defined, False otherwise.

                )doc"
            )

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
                "get_read_coordinates_subsets",
                &ConstantThrust::getReadCoordinatesSubsets,
                R"doc(
                    Get the subsets of coordinates read by the constant thrust.

                    Returns:
                        list: The subsets of coordinates read by the constant thrust.

                )doc"
            )

            .def(
                "get_write_coordinates_subsets",
                &ConstantThrust::getWriteCoordinatesSubsets,
                R"doc(
                    Get the subsets of coordinates written by the constant thrust.

                    Returns:
                        list: The subsets of coordinates written by the constant thrust.

                )doc"
            )

            .def(
                "compute_contribution",
                &ConstantThrust::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution of the constant thrust to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                            frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution of the constant thrust to the state vector.

                )doc"
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

                    Group:
                        Static methods
                )doc",
                arg("satellite_system"),
                arg("velocity_direction") = true,
                arg("frame") = Frame::GCRF()
            )

            ;
    }
}
