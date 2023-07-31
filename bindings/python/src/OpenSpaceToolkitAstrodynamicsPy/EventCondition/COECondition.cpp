/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>

using namespace pybind11;

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::physics::units::Derived;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::COECondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(pybind11::module& aModule)
{
    {
        class_<COECondition, EventCondition> coeCondition(aModule, "COECondition");

        coeCondition

            .def(
                init<
                    const String&,
                    const EventCondition::Criteria&,
                    const COECondition::Element&,
                    const Real&,
                    const Derived&>(),
                arg("name"),
                arg("criteria"),
                arg("element"),
                arg("target"),
                arg("gravitational_parameter")
            )

            .def("get_target", &COECondition::getTarget)
            .def("get_gravitational_parameter", &COECondition::getGravitationalParameter)
            .def("get_element", &COECondition::getElement)

            .def_static("string_from_element", &COECondition::StringFromElement, arg("element"))

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

        enum_<COECondition::Element>(coeCondition, "Element")

            .value("SemiMajorAxis", COECondition::Element::SemiMajorAxis)
            .value("Eccentricity", COECondition::Element::Eccentricity)
            .value("Inclination", COECondition::Element::Inclination)
            .value("ArgumentOfPeriapsis", COECondition::Element::ArgumentOfPeriapsis)
            .value("RightAngleOfAscendingNode", COECondition::Element::RightAngleOfAscendingNode)
            .value("TrueAnomaly", COECondition::Element::TrueAnomaly)
            .value("MeanAnomaly", COECondition::Element::MeanAnomaly)
            .value("EccentricAnomaly", COECondition::Element::EccentricAnomaly)

            ;
    }
}
