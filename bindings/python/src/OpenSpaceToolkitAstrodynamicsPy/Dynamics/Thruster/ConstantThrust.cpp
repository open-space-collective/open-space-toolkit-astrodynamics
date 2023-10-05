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

            .def("compute_contribution", &ConstantThrust::computeContribution, arg("instant"), arg("x"), arg("frame"))

            .def_static(
                "intrack",
                &ConstantThrust::Intrack,
                arg("satellite_system"),
                arg("velocity_direction") = true,
                arg("frame") = Frame::GCRF()
            )

            ;
    }
}
