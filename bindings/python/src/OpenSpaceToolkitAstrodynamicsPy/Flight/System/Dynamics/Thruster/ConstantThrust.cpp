/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrust.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thruster_ConstantThrust(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::String;
    using ostk::core::types::Shared;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::trajectory::LocalOrbitalFrameDirection;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::Thruster;
    using ostk::astro::flight::system::dynamics::thruster::ConstantThrust;

    {
        class_<ConstantThrust, Thruster, Shared<ConstantThrust>>(aModule, "ConstantThrust")
            .def(
                init<const SatelliteSystem&, const LocalOrbitalFrameDirection&, const String&>(),
                arg("satellite_system"),
                arg("thrust_direction"),
                arg("name") = String::Empty()
            )

            // .def("__str__", &(shiftToString<ConstantThrust>))
            // .def("__repr__", &(shiftToString<ConstantThrust>))

            .def("is_defined", &ConstantThrust::isDefined)

            // .def("get_thrust", &ConstantThrust::getThrust)
            .def("get_local_thrust_direction", &ConstantThrust::getLocalThrustDirection)

            .def("get_read_coordinates_subsets", &ConstantThrust::getReadCoordinatesSubsets)
            .def("get_write_coordinates_subsets", &ConstantThrust::getWriteCoordinatesSubsets)

            .def("compute_contribution", &ConstantThrust::computeContribution, arg("instant"), arg("x"), arg("frame"));
    }
}
