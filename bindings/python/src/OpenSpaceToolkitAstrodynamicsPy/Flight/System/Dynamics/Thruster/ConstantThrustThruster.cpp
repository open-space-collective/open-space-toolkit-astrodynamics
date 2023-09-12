/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrustThruster.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thruster_ConstantThrustThruster(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::types::String;
    using ostk::core::types::Shared;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::trajectory::LocalOrbitalFrameDirection;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::Thruster;
    using ostk::astro::flight::system::dynamics::thruster::ConstantThrustThruster;

    {
        class_<ConstantThrustThruster, Thruster, Shared<ConstantThrustThruster>>(aModule, "ConstantThrustThruster")
            .def(
                init<const SatelliteSystem&, const LocalOrbitalFrameDirection&, const String&>(),
                arg("satellite_system"),
                arg("thrust_direction"),
                arg("name") = String::Empty()
            )

            // .def("__str__", &(shiftToString<ConstantThrustThruster>))
            // .def("__repr__", &(shiftToString<ConstantThrustThruster>))

            .def("is_defined", &ConstantThrustThruster::isDefined)

            // .def("get_thrust", &ConstantThrustThruster::getThrust)

            .def("get_read_coordinates_subsets", &ConstantThrustThruster::getReadCoordinatesSubsets)
            .def("get_write_coordinates_subsets", &ConstantThrustThruster::getWriteCoordinatesSubsets)

            .def(
                "compute_contribution",
                &ConstantThrustThruster::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame")
            );
    }
}
