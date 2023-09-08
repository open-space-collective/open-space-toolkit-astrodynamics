/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>

using namespace pybind11;

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::physics::units::Derived;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::COECondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(pybind11::module& aModule)
{
    {
        class_<COECondition>(aModule, "COECondition")

            .def_static(
                "semi_major_axis",
                &COECondition::SemiMajorAxis,
                arg("criterion"),
                arg("frame"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentricity",
                &COECondition::Eccentricity,
                arg("criterion"),
                arg("frame"),
                arg("eccentricity"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                &COECondition::Inclination,
                arg("criterion"),
                arg("frame"),
                arg("inclination"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop", &COECondition::Aop, arg("criterion"), arg("frame"), arg("aop"), arg("gravitational_parameter")

            )

            .def_static(
                "raan", &COECondition::Raan, arg("criterion"), arg("frame"), arg("raan"), arg("gravitational_parameter")

            )

            .def_static(
                "true_anomaly",
                &COECondition::TrueAnomaly,
                arg("criterion"),
                arg("frame"),
                arg("true_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                &COECondition::MeanAnomaly,
                arg("criterion"),
                arg("frame"),
                arg("mean_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                &COECondition::EccentricAnomaly,
                arg("criterion"),
                arg("frame"),
                arg("eccentric_anomaly"),
                arg("gravitational_parameter")
            )

            ;
    }
}
