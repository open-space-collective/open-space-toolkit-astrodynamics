////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE (        pybind11::module& aModule                           )
{

    using namespace pybind11 ;

    using ostk::core::types::Real ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::kepler::COE ;

    class_<COE>(aModule, "COE")

        .def(init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>())

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
        .def("get_mean_motion", &COE::getMeanMotion)
        .def("get_orbital_period", &COE::getOrbitalPeriod)
        .def("get_cartesian_state", &COE::getCartesianState)

        .def_static("undefined", &COE::Undefined)
        .def_static("cartesian", &COE::Cartesian)
        .def_static("eccentric_anomaly_from_true_anomaly", &COE::EccentricAnomalyFromTrueAnomaly)
        .def_static("true_anomaly_from_eccentric_anomaly", &COE::TrueAnomalyFromEccentricAnomaly)
        .def_static("mean_anomaly_from_eccentric_anomaly", &COE::MeanAnomalyFromEccentricAnomaly)
        .def_static("eccentric_anomaly_from_mean_anomaly", &COE::EccentricAnomalyFromMeanAnomaly)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
