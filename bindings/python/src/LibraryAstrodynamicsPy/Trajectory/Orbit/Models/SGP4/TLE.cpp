////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Models/SGP4/TLE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE ( )
{

    using namespace boost::python ;

    using library::core::types::String ;

    using library::astro::trajectory::orbit::models::sgp4::TLE ;

    scope in_TLE = class_<TLE>("TLE", init<String, String>())

        .def(init<String, String, String>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &TLE::isDefined)

        .def("getSatelliteName", &TLE::getSatelliteName)
        .def("getFirstLine", &TLE::getFirstLine)
        .def("getSecondLine", &TLE::getSecondLine)
        .def("getSatelliteNumber", &TLE::getSatelliteNumber)
        .def("getClassification", &TLE::getClassification)
        .def("getInternationalDesignator", &TLE::getInternationalDesignator)
        .def("getEpoch", &TLE::getEpoch)
        .def("getMeanMotionFirstTimeDerivativeDividedByTwo", &TLE::getMeanMotionFirstTimeDerivativeDividedByTwo)
        .def("getMeanMotionSecondTimeDerivativeDividedBySix", &TLE::getMeanMotionSecondTimeDerivativeDividedBySix)
        .def("getBStarDragTerm", &TLE::getBStarDragTerm)
        .def("getEphemerisType", &TLE::getEphemerisType)
        .def("getElementSetNumber", &TLE::getElementSetNumber)
        .def("getFirstLineChecksum", &TLE::getFirstLineChecksum)
        .def("getInclination", &TLE::getInclination)
        .def("getRaan", &TLE::getRaan)
        .def("getEccentricity", &TLE::getEccentricity)
        .def("getAop", &TLE::getAop)
        .def("getMeanAnomaly", &TLE::getMeanAnomaly)
        .def("getMeanMotion", &TLE::getMeanMotion)
        .def("getRevolutionNumberAtEpoch", &TLE::getRevolutionNumberAtEpoch)

        .def("Undefined", &TLE::Undefined).staticmethod("Undefined")
        .def("CanParse", +[] (const String& aFirstLine, const String& aSecondLine) -> bool { return TLE::CanParse(aFirstLine, aSecondLine) ; })
        .def("Parse", &TLE::Parse).staticmethod("Parse")
        .def("Load", &TLE::Load).staticmethod("Load")

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
