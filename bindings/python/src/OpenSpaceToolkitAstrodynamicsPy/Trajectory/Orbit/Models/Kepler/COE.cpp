/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::kepler::COE;

    class_<COE> coe(
        aModule,
        "COE",
        R"doc(
            Classical orbital elements.

            Provides the classical orbital elements used to describe the orbit of a body around another.

            .. math::

                \begin{aligned}
                    a & = \text{semi-major axis} \\
                    e & = \text{eccentricity} \\
                    i & = \text{inclination} \\
                    \Omega & = \text{right ascension of the ascending node} \\
                    \omega & = \text{argument of periapsis} \\
                    \nu & = \text{true anomaly} \\
                    M & = \text{mean anomaly} \\
                    E & = \text{eccentric anomaly} \\
                    r_p & = \text{periapsis radius} \\
                    r_a & = \text{apoapsis radius}
                \end{aligned}

            Group:
                kepler
        )doc"
    );

    coe

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            R"doc(
                Constructor.

                Args:
                    semi_major_axis (Length): The semi-major axis.
                    eccentricity (Real): The eccentricity.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    aop (Angle): The argument of periapsis.
                    true_anomaly (Angle): The true anomaly.

                Group:
                    Constructors
            )doc",
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("inclination"),
            arg("raan"),
            arg("aop"),
            arg("true_anomaly")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<COE>))
        .def("__repr__", &(shiftToString<COE>))

        .def("is_defined", &COE::isDefined)

        .def("get_semi_major_axis", &COE::getSemiMajorAxis)
        .def("get_eccentricity", &COE::getEccentricity)
        .def("get_inclination", &COE::getInclination)
        .def("get_raan", &COE::getRaan)
        .def("get_aop", &COE::getAop)
        .def("get_true_anomaly", &COE::getTrueAnomaly)
        .def("get_mean_anomaly", &COE::getMeanAnomaly)
        .def("get_eccentric_anomaly", &COE::getEccentricAnomaly)
        .def("get_periapsis_radius", &COE::getPeriapsisRadius)
        .def("get_apoapsis_radius", &COE::getApoapsisRadius)
        .def("get_SI_vector", &COE::getSIVector, arg("anomaly_type"))

        .def("get_mean_motion", &COE::getMeanMotion, arg("gravitational_parameter"))

        .def("get_orbital_period", &COE::getOrbitalPeriod, arg("gravitational_parameter"))

        .def("get_cartesian_state", &COE::getCartesianState, arg("gravitational_parameter"), arg("frame"))

        .def_static("undefined", &COE::Undefined)

        .def_static(
            "cartesian",
            &COE::Cartesian,
            R"doc(
                Create a `COE` model from Cartesian state.

                Args:
                    cartesian_state (CartesianState): The Cartesian state.
                    gravitational_parameter (Real): The gravitational parameter of the central body.

                Returns:
                    COE: The `COE` model.

                Group:
                    Static methods
            )doc",
            arg("cartesian_state"),
            arg("gravitational_parameter")
        )

        .def_static(
            "from_SI_vector",
            &COE::FromSIVector,
            R"doc(
                Create a `COE` model from a state vector in SI units.

                Args:
                    vector (Vector6d): The state vector.
                    anomaly_type (AnomalyType): The type of anomaly.

                Returns:
                    COE: The `COE` model.

                Group:
                    Static methods
            )doc",
            arg("vector"),
            arg("anomaly_type")
        )

        .def_static(
            "eccentric_anomaly_from_true_anomaly",
            &COE::EccentricAnomalyFromTrueAnomaly,
            R"doc(
                Compute the eccentric anomaly from the true anomaly.

                Args:
                    true_anomaly (Angle): The true anomaly.
                    eccentricity (Real): The eccentricity.

                Returns:
                    Angle: The eccentric anomaly.

                Group:
                    Static methods
            )doc",
            arg("true_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "true_anomaly_from_eccentric_anomaly",
            &COE::TrueAnomalyFromEccentricAnomaly,
            R"doc(
                Compute the true anomaly from the eccentric anomaly.

                Args:
                    eccentric_anomaly (Angle): The eccentric anomaly.
                    eccentricity (Real): The eccentricity.

                Returns:
                    Angle: The true anomaly.

                Group:
                    Static methods
            )doc",
            arg("eccentric_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "mean_anomaly_from_eccentric_anomaly",
            &COE::MeanAnomalyFromEccentricAnomaly,
            R"doc(
                Compute the mean anomaly from the eccentric anomaly.

                Args:
                    eccentric_anomaly (Angle): The eccentric anomaly.
                    eccentricity (Real): The eccentricity.

                Returns:
                    Angle: The mean anomaly.

                Group:
                    Static methods
            )doc",
            arg("eccentric_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "eccentric_anomaly_from_mean_anomaly",
            &COE::EccentricAnomalyFromMeanAnomaly,
            R"doc(
                Compute the eccentric anomaly from the mean anomaly.

                Args:
                    mean_anomaly (Angle): The mean anomaly.
                    eccentricity (Real): The eccentricity.
                    tolerance (float): The tolerance of the root solver.

                Returns:
                    Angle: The eccentric anomaly.

                Group:
                    Static methods
            )doc",
            arg("mean_anomaly"),
            arg("eccentricity"),
            arg("tolerance")
        )

        .def_static(
            "true_anomaly_from_mean_anomaly",
            &COE::TrueAnomalyFromMeanAnomaly,
            R"doc(
                Compute the true anomaly from the mean anomaly.

                Args:
                    mean_anomaly (Angle): The mean anomaly.
                    eccentricity (Real): The eccentricity.
                    tolerance (float): The tolerance of the root solver.

                Returns:
                    Angle: The true anomaly.

                Group:
                    Static methods
            )doc",
            arg("mean_anomaly"),
            arg("eccentricity"),
            arg("tolerance")
        )

        .def_static(
            "string_from_element",
            &COE::StringFromElement,
            R"doc(
                Get the string representation of an element.

                Args:
                    element (Element): The element.

                Returns:
                    str: The string representation.

                Group:
                    Static methods
            )doc",
            arg("element")
        )

        ;

    enum_<COE::Element>(
        coe,
        "Element",
        R"doc(
            Classical Orbital Element enumeration.

        )doc"
    )

        .value("SemiMajorAxis", COE::Element::SemiMajorAxis, "Semi-Major Axis")
        .value("Eccentricity", COE::Element::Eccentricity, "Eccentricity")
        .value("Inclination", COE::Element::Inclination, "Inclination")
        .value("Aop", COE::Element::Aop, "Argument of Perigee")
        .value("Raan", COE::Element::Raan, "Right Angle of the Ascending Node")
        .value("TrueAnomaly", COE::Element::TrueAnomaly, "True Anomaly")
        .value("MeanAnomaly", COE::Element::MeanAnomaly, "Mean Anomaly")
        .value("EccentricAnomaly", COE::Element::EccentricAnomaly, "Eccentric Anomaly")

        ;

    enum_<COE::AnomalyType>(
        coe,
        "AnomalyType",
        R"doc(
            The type of Anomaly.
        )doc"
    )

        // Have to rename slightly as True is a keyword in Python
        .value("TrueAnomaly", COE::AnomalyType::True, "True Anomaly")
        .value("MeanAnomaly", COE::AnomalyType::Mean, "Mean Anomaly")
        .value("EccentricAnomaly", COE::AnomalyType::Eccentric, "Eccentric Anomaly")

        ;
}
