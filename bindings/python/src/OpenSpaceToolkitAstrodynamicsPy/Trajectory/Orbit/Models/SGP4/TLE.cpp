////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4/TLE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE (        pybind11::module& aModule                             )
{

    using namespace pybind11 ;

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    class_<TLE>(aModule, "TLE")

        .def(init<String, String>())

        .def(init<String, String, String>())

        .def(self == self)
        .def(self != self)

        // .def(self_ns::str(self_ns::self))
        // .def(self_ns::repr(self_ns::self))

        .def("is_defined", &TLE::isDefined)

        .def("get_satellite_name", &TLE::getSatelliteName)
        .def("get_first_line", &TLE::getFirstLine)
        .def("get_second_line", &TLE::getSecondLine)
        .def("get_satellite_number", &TLE::getSatelliteNumber)
        .def("get_classification", &TLE::getClassification)
        .def("get_international_designator", &TLE::getInternationalDesignator)
        .def("get_epoch", &TLE::getEpoch)
        .def("get_mean_motion_first_time_derivative_divided_by_two", &TLE::getMeanMotionFirstTimeDerivativeDividedByTwo)
        .def("get_mean_motion_second_time_derivative_divided_by_six", &TLE::getMeanMotionSecondTimeDerivativeDividedBySix)
        .def("get_b_star_drag_term", &TLE::getBStarDragTerm)
        .def("get_ephemeris_type", &TLE::getEphemerisType)
        .def("get_element_set_number", &TLE::getElementSetNumber)
        .def("get_first_line_checksum", &TLE::getFirstLineChecksum)
        .def("get_inclination", &TLE::getInclination)
        .def("get_raan", &TLE::getRaan)
        .def("get_eccentricity", &TLE::getEccentricity)
        .def("get_aop", &TLE::getAop)
        .def("get_mean_anomaly", &TLE::getMeanAnomaly)
        .def("get_mean_motion", &TLE::getMeanMotion)
        .def("get_revolution_number_at_epoch", &TLE::getRevolutionNumberAtEpoch)
        .def("get_second_line_checksum", &TLE::getSecondLineChecksum)

        .def_static("undefined", &TLE::Undefined)
        .def("can_parse", +[] (const String& aFirstLine, const String& aSecondLine) -> bool { return TLE::CanParse(aFirstLine, aSecondLine) ; })
        .def_static("parse", &TLE::Parse)
        .def_static("load", &TLE::Load)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
