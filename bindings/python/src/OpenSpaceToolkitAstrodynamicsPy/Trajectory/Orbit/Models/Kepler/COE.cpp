/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::kepler::COE;

    class_<COE> coe(aModule, "COE");

    coe

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
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

        .def_static("cartesian", &COE::Cartesian, arg("cartesian_state"), arg("gravitational_parameter"))

        .def_static("from_SI_vector", &COE::FromSIVector, arg("vector"), arg("anomaly_type"))

        .def_static(
            "eccentric_anomaly_from_true_anomaly",
            &COE::EccentricAnomalyFromTrueAnomaly,
            arg("true_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "true_anomaly_from_eccentric_anomaly",
            &COE::TrueAnomalyFromEccentricAnomaly,
            arg("eccentric_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "mean_anomaly_from_eccentric_anomaly",
            &COE::MeanAnomalyFromEccentricAnomaly,
            arg("eccentric_anomaly"),
            arg("eccentricity")
        )

        .def_static(
            "eccentric_anomaly_from_mean_anomaly",
            &COE::EccentricAnomalyFromMeanAnomaly,
            arg("mean_anomaly"),
            arg("eccentricity"),
            arg("tolerance")
        )

        .def_static(
            "true_anomaly_from_mean_anomaly",
            &COE::TrueAnomalyFromMeanAnomaly,
            arg("mean_anomaly"),
            arg("eccentricity"),
            arg("tolerance")
        )

        .def_static("string_from_element", &COE::StringFromElement, arg("element"))

        ;

    enum_<COE::Element>(coe, "Element")

        .value("SemiMajorAxis", COE::Element::SemiMajorAxis)
        .value("Eccentricity", COE::Element::Eccentricity)
        .value("Inclination", COE::Element::Inclination)
        .value("Aop", COE::Element::Aop)
        .value("Raan", COE::Element::Raan)
        .value("TrueAnomaly", COE::Element::TrueAnomaly)
        .value("MeanAnomaly", COE::Element::MeanAnomaly)
        .value("EccentricAnomaly", COE::Element::EccentricAnomaly)

        ;

    enum_<COE::AnomalyType>(coe, "AnomalyType")

        .value("TrueAnomaly", COE::AnomalyType::True)
        .value("MeanAnomaly", COE::AnomalyType::Mean)
        .value("EccentricAnomaly", COE::AnomalyType::Eccentric)

        ;
}
