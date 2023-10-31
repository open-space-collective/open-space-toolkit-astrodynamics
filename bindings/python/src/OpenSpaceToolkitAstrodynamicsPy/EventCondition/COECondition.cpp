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
        class_<COECondition>(
            aModule,
            "COECondition",
            R"doc(
                A COE Event Condition.

                Group:
                    event-condition
            )doc"
        )

            .def_static(
                "semi_major_axis",
                &COECondition::SemiMajorAxis,
                R"doc(
                    Create a COE condition based on the semi-major axis.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        semi_major_axis (float): The semi-major axis.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentricity",
                &COECondition::Eccentricity,
                R"doc(
                    Create a COE condition based on the eccentricity.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentricity (float): The eccentricity.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("eccentricity"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                &COECondition::Inclination,
                R"doc(
                    Create a COE condition based on the inclination.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        inclination (float): The inclination.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("inclination"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop",
                &COECondition::Aop,
                R"doc(
                    Create a COE condition based on the argument of perigee.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        aop (float): The argument of perigee.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("aop"),
                arg("gravitational_parameter")
            )

            .def_static(
                "raan",
                &COECondition::Raan,
                R"doc(
                    Create a COE condition based on the right ascension of the ascending node.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        raan (float): The right ascension of the ascending node.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("raan"),
                arg("gravitational_parameter")
            )

            .def_static(
                "true_anomaly",
                &COECondition::TrueAnomaly,
                R"doc(
                    Create a COE condition based on the true anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        true_anomaly (float): The true anomaly.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("true_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                &COECondition::MeanAnomaly,
                R"doc(
                    Create a COE condition based on the mean anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        mean_anomaly (float): The mean anomaly.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("mean_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                &COECondition::EccentricAnomaly,
                R"doc(
                    Create a COE condition based on the eccentric anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentric_anomaly (float): The eccentric anomaly.
                        gravitational_parameter (float): The gravitational parameter.

                    Returns:
                        COECondition: The COE condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("eccentric_anomaly"),
                arg("gravitational_parameter")
            )

            ;
    }
}
