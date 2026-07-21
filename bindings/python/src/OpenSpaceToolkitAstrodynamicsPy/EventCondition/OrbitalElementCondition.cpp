/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/OrbitalElementCondition.hpp>

using namespace pybind11;

using ostk::core::container::Pair;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::OrbitalElementCondition;
using ostk::astrodynamics::eventcondition::RealCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_OrbitalElementCondition(pybind11::module& aModule)
{
    {
        class_<OrbitalElementCondition> orbitalElementCondition(
            aModule,
            "OrbitalElementCondition",
            R"doc(
                An orbital element based Event Condition. Supports evaluating the element using different orbital
                element theories (osculating or Brouwer-Lyddane mean).

            )doc"
        );

        enum_<OrbitalElementCondition::Theory>(
            orbitalElementCondition,
            "Theory",
            R"doc(
                The orbital element theory used to compute the orbital elements from a Cartesian state.
            )doc"
        )

            .value("Osculating", OrbitalElementCondition::Theory::Osculating, "Osculating (Classical) orbital elements")
            .value(
                "BrouwerLyddaneMeanShort",
                OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort,
                "Brouwer-Lyddane Mean Short orbital elements"
            )
            .value(
                "BrouwerLyddaneMeanLong",
                OrbitalElementCondition::Theory::BrouwerLyddaneMeanLong,
                "Brouwer-Lyddane Mean Long orbital elements"
            )

            ;

        orbitalElementCondition

            .def_static(
                "semi_major_axis",
                &OrbitalElementCondition::SemiMajorAxis,
                R"doc(
                    Create an orbital element condition based on the semi-major axis.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        semi_major_axis (EventConditionTarget): The semi-major axis.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentricity",
                &OrbitalElementCondition::Eccentricity,
                R"doc(
                    Create an orbital element condition based on the eccentricity.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentricity (EventConditionTarget): The eccentricity.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("eccentricity"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::Inclination),
                R"doc(
                    Create an orbital element condition based on the inclination.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        inclination (EventConditionTarget): The inclination.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("inclination"),
                arg("gravitational_parameter")
            )

            .def_static(
                "inclination",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::Inclination),
                R"doc(
                    Create an orbital element condition based on the inclination being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::Aop),
                R"doc(
                    Create an orbital element condition based on the argument of perigee.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        aop (EventConditionTarget): The argument of perigee.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("aop"),
                arg("gravitational_parameter")
            )

            .def_static(
                "aop",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::Aop),
                R"doc(
                    Create an orbital element condition based on the argument of perigee being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "raan",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::Raan),
                R"doc(
                    Create an orbital element condition based on the right ascension of the ascending node.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        raan (EventConditionTarget): The right ascension of the ascending node.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("raan"),
                arg("gravitational_parameter")
            )

            .def_static(
                "raan",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::Raan),
                R"doc(
                    Create an orbital element condition based on the right ascension of the ascending node being
                    within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "true_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::TrueAnomaly),
                R"doc(
                    Create an orbital element condition based on the true anomaly.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        true_anomaly (EventConditionTarget): The true anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("true_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "true_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::TrueAnomaly),
                R"doc(
                    Create an orbital element condition based on the true anomaly being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::MeanAnomaly),
                R"doc(
                    Create an orbital element condition based on the mean anomaly.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        mean_anomaly (EventConditionTarget): The mean anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("mean_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "mean_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::MeanAnomaly),
                R"doc(
                    Create an orbital element condition based on the mean anomaly being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::EccentricAnomaly),
                R"doc(
                    Create an orbital element condition based on the eccentric anomaly.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        eccentric_anomaly (EventConditionTarget): The eccentric anomaly.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("eccentric_anomaly"),
                arg("gravitational_parameter")
            )

            .def_static(
                "eccentric_anomaly",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::EccentricAnomaly),
                R"doc(
                    Create an orbital element condition based on the eccentric anomaly being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            .def_static(
                "argument_of_latitude",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const AngularCondition::Criterion&,
                    const Shared<const Frame>&,
                    const EventCondition::Target&,
                    const Derived&>(&OrbitalElementCondition::ArgumentOfLatitude),
                R"doc(
                    Create an orbital element condition based on the argument of latitude.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        criterion (Criterion): The criterion.
                        frame (Frame): The reference frame.
                        argument_of_latitude (EventConditionTarget): The argument of latitude.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("criterion"),
                arg("frame"),
                arg("argument_of_latitude"),
                arg("gravitational_parameter")
            )

            .def_static(
                "argument_of_latitude",
                overload_cast<
                    const OrbitalElementCondition::Theory&,
                    const Shared<const Frame>&,
                    const Pair<Angle, Angle>&,
                    const Derived&>(&OrbitalElementCondition::ArgumentOfLatitude),
                R"doc(
                    Create an orbital element condition based on the argument of latitude being within a range.

                    Args:
                        theory (OrbitalElementCondition.Theory): The orbital element theory to use.
                        frame (Frame): The reference frame.
                        target_range (tuple[Angle, Angle]): A tuple of two angles defining the range.
                        gravitational_parameter (Derived): The gravitational parameter.

                    Returns:
                        OrbitalElementCondition: The orbital element condition.
                )doc",
                arg("theory"),
                arg("frame"),
                arg("target_range"),
                arg("gravitational_parameter")
            )

            ;
    }
}
