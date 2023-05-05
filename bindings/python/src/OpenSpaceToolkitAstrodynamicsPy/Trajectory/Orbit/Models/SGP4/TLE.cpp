// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Instant;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    class_<TLE>(aModule, "TLE")

        .def(init<String, String>(), arg("first_line"), arg("second_line"))

        .def(init<String, String, String>(), arg("satellite_name"), arg("first_line"), arg("second_line"))

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<TLE>))
        .def("__repr__", &(shiftToString<TLE>))

        .def("is_defined", &TLE::isDefined)

        .def("get_satellite_name", &TLE::getSatelliteName)
        .def("get_first_line", &TLE::getFirstLine)
        .def("get_second_line", &TLE::getSecondLine)
        .def("get_satellite_number", &TLE::getSatelliteNumber)
        .def("get_classification", &TLE::getClassification)
        .def("get_international_designator", &TLE::getInternationalDesignator)
        .def("get_epoch", &TLE::getEpoch)
        .def("get_mean_motion_first_time_derivative_divided_by_two", &TLE::getMeanMotionFirstTimeDerivativeDividedByTwo)
        .def(
            "get_mean_motion_second_time_derivative_divided_by_six", &TLE::getMeanMotionSecondTimeDerivativeDividedBySix
        )
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

        .def("set_satellite_number", &TLE::setSatelliteNumber, arg("satellite_number"))

        .def("set_epoch", &TLE::setEpoch, arg("epoch"))

        .def("set_revolution_number_at_epoch", &TLE::setRevolutionNumberAtEpoch, arg("revolution_number"))

        .def_static("undefined", &TLE::Undefined)

        .def_static(
            "can_parse",
            +[](const String& aFirstLine, const String& aSecondLine) -> bool {
                return TLE::CanParse(aFirstLine, aSecondLine);
            },
            arg("first_line"),
            arg("second_line")
        )

        .def_static("parse", &TLE::Parse, arg("string"))

        .def_static("load", &TLE::Load, arg("file"))

        .def_static(
            "construct",
            overload_cast<
                const String&,
                const Integer&,
                const String&,
                const String&,
                const Instant&,
                const Real&,
                const Real&,
                const Real&,
                const Integer&,
                const Integer&,
                const Angle&,
                const Angle&,
                const Real&,
                const Angle&,
                const Angle&,
                const Derived&,
                const Integer&>(&TLE::Construct),
            arg("satellite_name"),
            arg("satellite_number"),
            arg("classification"),
            arg("international_designator"),
            arg("epoch"),
            arg("mean_motion_first_time_derivative_divided_by_two"),
            arg("mean_motion_second_time_derivative_divided_by_six"),
            arg("b_star_drag_term"),
            arg("ephemeris_type"),
            arg("element_set_number"),
            arg("inclination"),
            arg("raan"),
            arg("eccentricity"),
            arg("aop"),
            arg("mean_anomaly"),
            arg("mean_motion"),
            arg("revolution_number_at_epoch")
        )

        .def_static(
            "construct",
            overload_cast<
                const Integer&,
                const String&,
                const String&,
                const Instant&,
                const Real&,
                const Real&,
                const Real&,
                const Integer&,
                const Integer&,
                const Angle&,
                const Angle&,
                const Real&,
                const Angle&,
                const Angle&,
                const Derived&,
                const Integer&>(&TLE::Construct),
            arg("satellite_number"),
            arg("classification"),
            arg("international_designator"),
            arg("epoch"),
            arg("mean_motion_first_time_derivative_divided_by_two"),
            arg("mean_motion_second_time_derivative_divided_by_six"),
            arg("b_star_drag_term"),
            arg("ephemeris_type"),
            arg("element_set_number"),
            arg("inclination"),
            arg("raan"),
            arg("eccentricity"),
            arg("aop"),
            arg("mean_anomaly"),
            arg("mean_motion"),
            arg("revolution_number_at_epoch")
        )

        .def_static("generate_checksum", &TLE::GenerateChecksum, arg("string"))

        ;
}
