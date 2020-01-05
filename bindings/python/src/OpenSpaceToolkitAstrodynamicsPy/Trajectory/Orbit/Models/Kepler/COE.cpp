////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE ( )
{

    using namespace boost::python ;

    using ostk::core::types::Real ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::kepler::COE ;

    scope in_COE = class_<COE>("COE", init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &COE::isDefined)

        .def("getSemiMajorAxis", &COE::getSemiMajorAxis)
        .def("getEccentricity", &COE::getEccentricity)
        .def("getInclination", &COE::getInclination)
        .def("getRaan", &COE::getRaan)
        .def("getAop", &COE::getAop)
        .def("getTrueAnomaly", &COE::getTrueAnomaly)
        .def("getMeanAnomaly", &COE::getMeanAnomaly)
        .def("getEccentricAnomaly", &COE::getEccentricAnomaly)
        .def("getMeanMotion", &COE::getMeanMotion)
        .def("getOrbitalPeriod", &COE::getOrbitalPeriod)
        .def("getCartesianState", &COE::getCartesianState)

        .def("Undefined", &COE::Undefined).staticmethod("Undefined")
        .def("Cartesian", &COE::Cartesian).staticmethod("Cartesian")
        .def("EccentricAnomalyFromTrueAnomaly", &COE::EccentricAnomalyFromTrueAnomaly).staticmethod("EccentricAnomalyFromTrueAnomaly")
        .def("TrueAnomalyFromEccentricAnomaly", &COE::TrueAnomalyFromEccentricAnomaly).staticmethod("TrueAnomalyFromEccentricAnomaly")
        .def("MeanAnomalyFromEccentricAnomaly", &COE::MeanAnomalyFromEccentricAnomaly).staticmethod("MeanAnomalyFromEccentricAnomaly")
        .def("EccentricAnomalyFromMeanAnomaly", &COE::EccentricAnomalyFromMeanAnomaly).staticmethod("EccentricAnomalyFromMeanAnomaly")

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
