/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Kepler_COE(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Real;

    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Length;

    using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

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

        )doc"
    );

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

    coe

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            R"doc(
                Constructor.

                Args:
                    semi_major_axis (Length): The semi-major axis.
                    eccentricity (float): The eccentricity.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    aop (Angle): The argument of periapsis.
                    true_anomaly (Angle): The true anomaly.

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

        .def(
            "is_defined",
            &COE::isDefined,
            R"doc(
                Check if the COE is defined.

                Returns:
                    bool: True if the COE is defined, False otherwise.
            )doc"
        )

        .def(
            "get_semi_major_axis",
            &COE::getSemiMajorAxis,
            R"doc(
                Get the semi-major axis of the COE.

                Returns:
                    Length: The semi-major axis of the COE.
            )doc"
        )

        .def(
            "get_eccentricity",
            &COE::getEccentricity,
            R"doc(
                Get the eccentricity of the COE.

                Returns:
                    float: The eccentricity of the COE.
            )doc"
        )

        .def(
            "get_inclination",
            &COE::getInclination,
            R"doc(
                Get the inclination of the COE.

                Returns:
                    Angle: The inclination of the COE.
            )doc"
        )

        .def(
            "get_raan",
            &COE::getRaan,
            R"doc(
                Get the right ascension of the ascending node of the COE.

                Returns:
                    Angle: The right ascension of the ascending node of the COE.
            )doc"
        )

        .def(
            "get_aop",
            &COE::getAop,
            R"doc(
                Get the argument of periapsis of the COE.

                Returns:
                    Angle: The argument of periapsis of the COE.
            )doc"
        )

        .def(
            "get_true_anomaly",
            &COE::getTrueAnomaly,
            R"doc(
                Get the true anomaly of the COE.

                Returns:
                    Angle: The true anomaly of the COE.
            )doc"
        )

        .def(
            "get_mean_anomaly",
            &COE::getMeanAnomaly,
            R"doc(
                Get the mean anomaly of the COE.

                Returns:
                    Angle: The mean anomaly of the COE.
            )doc"
        )

        .def(
            "get_eccentric_anomaly",
            &COE::getEccentricAnomaly,
            R"doc(
                Get the eccentric anomaly of the COE.

                Returns:
                    Angle: The eccentric anomaly of the COE.
            )doc"
        )

        .def(
            "get_periapsis_radius",
            &COE::getPeriapsisRadius,
            R"doc(
                Get the periapsis radius of the COE.

                Returns:
                    Length: The periapsis radius of the COE.
            )doc"
        )

        .def(
            "get_apoapsis_radius",
            &COE::getApoapsisRadius,
            R"doc(
                Get the apoapsis radius of the COE.

                Returns:
                    Length: The apoapsis radius of the COE.
            )doc"
        )

        .def(
            "get_semi_latus_rectum",
            &COE::getSemiLatusRectum,
            R"doc(
               Get the semi-latus rectum of the COE.

               Returns:
                  Length: The semilatus rectum of the COE.
            )doc"
        )

        .def(
            "get_radial_distance",
            &COE::getRadialDistance,
            R"doc(
               Get the radial distance of the COE.

               Returns:
                  Length: The radial distance of the COE.
            )doc"
        )

        .def(
            "get_angular_momentum",
            &COE::getAngularMomentum,
            R"doc(
                Get the angular momentum of the COE.

                Args:
                    gravitational_parameter (Derived): The gravitational parameter of the central body.

                Returns:
                    Derived: The angular momentum of the COE.
            )doc"
        )

        .def(
            "get_SI_vector",
            &COE::getSIVector,
            R"doc(
               Get the state vector of the COE in the specified anomaly type.

               Args:
                  anomaly_type (AnomalyType): The type of anomaly.

               Returns:
                  numpy.ndarray: The state vector of the COE in the specified anomaly type.
            )doc",
            arg("anomaly_type")
        )

        .def(
            "get_mean_motion",
            &COE::getMeanMotion,
            R"doc(
               Get the mean motion of the COE.

               Args:
                  gravitational_parameter (Derived): The gravitational parameter of the central body.

               Returns:
                  Derived: The mean motion of the COE.
            )doc",
            arg("gravitational_parameter")
        )

        .def(
            "get_nodal_precession_rate",
            &COE::getNodalPrecessionRate,
            R"doc(
               Get the nodal precession of the COE.

               Args:
                  gravitational_parameter (Derived): The gravitational parameter of the central body.
                  equatorial_radius (Length): The equatorial radius of the central body.
                  j2 (float): The second zonal harmonic coefficient of the central body.

               Returns:
                  Derived: The nodal precession of the COE.
            )doc",
            arg("gravitational_parameter"),
            arg("equatorial_radius"),
            arg("j2")
        )

        .def(
            "get_orbital_period",
            &COE::getOrbitalPeriod,
            R"doc(
               Get the orbital period of the COE.

               Args:
                  gravitational_parameter (double): The gravitational parameter of the central body.

               Returns:
                  Duration: The orbital period of the COE.
            )doc",
            arg("gravitational_parameter")
        )

        .def(
            "get_cartesian_state",
            &COE::getCartesianState,
            R"doc(
               Get the Cartesian state of the COE.

               Args:
                  gravitational_parameter (double): The gravitational parameter of the central body.
                  frame (Frame): The reference frame in which to express the Cartesian state.

               Returns:
                  CartesianState: The Cartesian state of the COE.
            )doc",
            arg("gravitational_parameter"),
            arg("frame")
        )

        .def_static(
            "undefined",
            &COE::Undefined,
            R"doc(
                Create an undefined `COE` model.

                Returns:
                    COE: The undefined `COE` model.
            )doc"
        )

        .def_static(
            "cartesian",
            &COE::Cartesian,
            R"doc(
                Create a `COE` model from Cartesian state.

                Args:
                    cartesian_state (CartesianState): The Cartesian state.
                    gravitational_parameter (float): The gravitational parameter of the central body.

                Returns:
                    COE: The `COE` model.
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
                    eccentricity (float): The eccentricity.

                Returns:
                    Angle: The eccentric anomaly.
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
                    eccentricity (float): The eccentricity.

                Returns:
                    Angle: The true anomaly.
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
                    eccentricity (float): The eccentricity.

                Returns:
                    Angle: The mean anomaly.
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
                    eccentricity (float): The eccentricity.
                    tolerance (float): The tolerance of the root solver.

                Returns:
                    Angle: The eccentric anomaly.
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
                    eccentricity (float): The eccentricity.
                    tolerance (float): The tolerance of the root solver.

                Returns:
                    Angle: The true anomaly.
            )doc",
            arg("mean_anomaly"),
            arg("eccentricity"),
            arg("tolerance")
        )

        .def_static(
            "compute_semi_latus_rectum",
            &COE::ComputeSemiLatusRectum,
            R"doc(
                Compute the semi-latus rectum from the semi-major axis and the eccentricity.

                Args:
                    semi_major_axis (float): The semi-major axis. In meters.
                    eccentricity (float): The eccentricity.

                Returns:
                    Length: The semi-latus rectum.
            )doc",
            arg("semi_major_axis"),
            arg("eccentricity")
        )

        .def_static(
            "compute_angular_momentum",
            overload_cast<const Real&, const Real&, const Derived&>(&COE::ComputeAngularMomentum),
            R"doc(
                Compute the angular momentum from the semi-major axis and the eccentricity.

                Args:
                    semi_major_axis (float): The semi-major axis. In meters.
                    eccentricity (float): The eccentricity.
                    gravitational_parameter (Derived): The gravitational parameter of the central body.

                Returns:
                    Derived: The angular momentum.
            )doc",
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("gravitational_parameter")
        )

        .def_static(
            "compute_angular_momentum",
            overload_cast<const Real&, const Derived&>(&COE::ComputeAngularMomentum),
            R"doc(
                Compute the angular momentum from the semi-latus rectum.

                Args:
                    semi_latus_rectum (float): The semi-latus rectum. In meters.
                    gravitational_parameter (Derived): The gravitational parameter of the central body.

                Returns:
                    Derived: The angular momentum.
            )doc",
            arg("semi_latus_rectum"),
            arg("gravitational_parameter")
        )

        .def_static(
            "compute_radial_distance",
            &COE::ComputeRadialDistance,
            R"doc(
                Compute the radial distance from the semi-latus rectum and the eccentricity.

                Args:
                    semi_latus_rectum (float): The semi-latus rectum. In meters.
                    eccentricity (float): The eccentricity.
                    true_anomaly (float): The true anomly. In degrees.

                Returns:
                    Length: The radial distance.
            )doc",
            arg("semi_latus_rectum"),
            arg("eccentricity"),
            arg("true_anomaly")
        )

        .def_static(
            "compute_mean_ltan",
            &COE::ComputeMeanLTAN,
            R"doc(
                Compute the Mean Local Time of the Ascending Node (MLTAN) from the RAAN and instant.

                Args:
                    raan (Angle): The Right Ascension of the Ascending Node.
                    instant (Instant): The instant at which to compute MLTAN.
                    sun (Sun): The Sun model.

                Returns:
                    float: The Mean Local Time of the Ascending Node (MLTAN) in hours.
            )doc",
            arg("raan"),
            arg("instant"),
            arg_v("sun", ostk::physics::environment::object::celestial::Sun::Default(), "Sun.default()")
        )

        .def_static(
            "compute_ltan",
            &COE::ComputeLTAN,
            R"doc(
                Compute the Local Time of the Ascending Node (LTAN) from the RAAN and instant.

                Args:
                    raan (Angle): The Right Ascension of the Ascending Node.
                    instant (Instant): The instant at which to compute LTAN.
                    sun (Sun): The Sun model.

                Returns:
                    float: The Local Time of the Ascending Node (LTAN) in hours.
            )doc",
            arg("raan"),
            arg("instant"),
            arg_v("sun", ostk::physics::environment::object::celestial::Sun::Default(), "Sun.default()")
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
            )doc",
            arg("element")
        )

        ;
}
