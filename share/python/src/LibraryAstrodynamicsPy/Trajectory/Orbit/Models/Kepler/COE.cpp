////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE ( )
{

    using namespace boost::python ;

    using library::core::types::Real ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;

    using library::astro::trajectory::orbit::models::kepler::COE ;

    scope in_COE = class_<COE>("COE", init<Length, Real, Angle, Angle, Angle, Angle>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))

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
        .def("print", &COE::print)
        
        .def("Undefined", &COE::Undefined).staticmethod("Undefined")
        .def("EccentricAnomalyFromTrueAnomaly", &COE::EccentricAnomalyFromTrueAnomaly).staticmethod("EccentricAnomalyFromTrueAnomaly")
        .def("TrueAnomalyFromEccentricAnomaly", &COE::TrueAnomalyFromEccentricAnomaly).staticmethod("TrueAnomalyFromEccentricAnomaly")
        .def("MeanAnomalyFromEccentricAnomaly", &COE::MeanAnomalyFromEccentricAnomaly).staticmethod("MeanAnomalyFromEccentricAnomaly")
        .def("EccentricAnomalyFromMeanAnomaly", &COE::EccentricAnomalyFromMeanAnomaly).staticmethod("EccentricAnomalyFromMeanAnomaly")

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////