/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BrouwerLyddaneMeanLongCondition.hpp>

using namespace pybind11;

using ostk::core::container::Pair;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::BrouwerLyddaneMeanLongCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_BrouwerLyddaneMeanLongCondition(pybind11::module& aModule)
{
    {
        class_<BrouwerLyddaneMeanLongCondition>(
            aModule,
            "BrouwerLyddaneMeanLongCondition",
            R"doc(
                A Brouwer-Lyddane Mean Long Event Condition.

            )doc"
        )

            .def_static(
                "semi_major_axis",
                &BrouwerLyddaneMeanLongCondition::SemiMajorAxis,
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the semi-major axis.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        semi_major_axis (EventConditionTarget): The semi-major axis.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentricity",
                &BrouwerLyddaneMeanLongCondition::Eccentricity,
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the eccentricity.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentricity (EventConditionTarget): The eccentricity.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("eccentricity"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::Inclination),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the inclination.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        inclination (EventConditionTarget): The inclination.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("inclination"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::Inclination
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the inclination being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::Aop),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the argument of perigee.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        aop (EventConditionTarget): The argument of perigee.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("aop"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::Aop
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the argument of perigee being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "raan",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::Raan),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the right ascension of the ascending node.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        raan (EventConditionTarget): The right ascension of the ascending node.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("raan"),
                arg("gravitational_parameter")
            )

            .def_static(
                "raan",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::Raan
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the right ascension of the ascending node being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "true_anomaly",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::TrueAnomaly),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the true anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        true_anomaly (EventConditionTarget): The true anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("true_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "true_anomaly",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::TrueAnomaly
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the true anomaly being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::MeanAnomaly),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the mean anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        mean_anomaly (EventConditionTarget): The mean anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("mean_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::MeanAnomaly
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the mean anomaly being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::EccentricAnomaly),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the eccentric anomaly.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentric_anomaly (EventConditionTarget): The eccentric anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("eccentric_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::EccentricAnomaly
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the eccentric anomaly being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "argument_of_latitude",
                overload_cast<
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&BrouwerLyddaneMeanLongCondition::ArgumentOfLatitude),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the argument of latitude.

                    Args:
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        argument_of_latitude (EventConditionTarget): The argument of latitude.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("criterion"),
                arg("frame"),
                arg("argument_of_latitude"),
                arg("gravitational_parameter")
            )

            .def_static(
                "argument_of_latitude",
                overload_cast<const Shared<const Frame>&, const Pair<Angle, Angle>&, const Derived&>(
                    &BrouwerLyddaneMeanLongCondition::ArgumentOfLatitude
                ),
                R"doc(
                    Create a Brouwer-Lyddane Mean Long condition based on the argument of latitude being within a range.

                    Args:
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        BrouwerLyddaneMeanLongCondition: The Brouwer-Lyddane Mean Long condition.
                )doc",
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            ;
    }
}
