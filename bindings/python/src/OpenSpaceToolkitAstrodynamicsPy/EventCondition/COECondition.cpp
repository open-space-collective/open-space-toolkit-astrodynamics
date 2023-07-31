/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::units::Length;
using ostk::physics::units::Derived;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::COECondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(pybind11::module& aModule)
{
    {
        class_<COECondition, Shared<COECondition>, EventCondition>(aModule, "COECondition")

            .def("get_target", &COECondition::getTarget)
            .def("get_gravitational_parameter", &COECondition::getGravitationalParameter)

            .def_static(
                "semi_major_axis",
                &COECondition::SemiMajorAxis,
                arg("criteria"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentricity",
                &COECondition::Eccentricity,
                arg("criteria"),
                arg("eccentricity"),
                arg("gravitational_parameter")

            )

            .def_static(
                "inclination",
                &COECondition::Inclination,
                arg("criteria"),
                arg("inclination"),
                arg("gravitational_parameter")
            )

            .def_static(
                "argument_of_periapsis",
                &COECondition::ArgumentOfPeriapsis,
                arg("criteria"),
                arg("argument_of_periapsis"),
                arg("gravitational_parameter")

            )

            .def_static(
                "right_angle_of_ascending_node",
                &COECondition::RightAngleOfAscendingNode,
                arg("criteria"),
                arg("right_angle_of_ascending_node"),
                arg("gravitational_parameter")

            )

            .def_static(
                "true_anomaly",
                &COECondition::TrueAnomaly,
                arg("criteria"),
                arg("true_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                &COECondition::MeanAnomaly,
                arg("criteria"),
                arg("mean_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                &COECondition::EccentricAnomaly,
                arg("criteria"),
                arg("eccentric_anomaly"),
                arg("gravitational_parameter")
            )

            ;
    }
}
