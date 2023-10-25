/// Apache License 2.0

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

    class_<TLE>(
        aModule,
        "TLE",
        R"doc(
            A Two-Line Element set (TLE).

            A TLE is a data format encoding a list of orbital elements of an Earth-orbiting object for a given point in time

            Reference:
                https://en.wikipedia.org/wiki/Two-line_element_set

            Group:
                orbit
        )doc"
    )

        .def(
            init<String, String>(),
            arg("first_line"),
            arg("second_line"),
            R"doc(
                Constructor.

                Args:
                    first_line (str): The first line of the TLE.
                    second_line (str): The second line of the TLE.

            )doc"
        )

        .def(
            init<String, String, String>(),
            arg("satellite_name"),
            arg("first_line"),
            arg("second_line"),
            R"doc(
                Constructor.

                Args:
                    satellite_name (str): The name of the satellite.
                    first_line (str): The first line of the TLE.
                    second_line (str): The second line of the TLE.

            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<TLE>))
        .def("__repr__", &(shiftToString<TLE>))

        .def(
            "is_defined",
            &TLE::isDefined,
            R"doc(
                Check if the `TLE` object is defined.

                Returns:
                    bool: True if the `TLE` object is defined, False otherwise.

            )doc"
        )

        .def(
            "get_satellite_name",
            &TLE::getSatelliteName,
            R"doc(
                Get the name of the satellite.

                Returns:
                    str: The name of the satellite.

            )doc"
        )
        .def(
            "get_first_line",
            &TLE::getFirstLine,
            R"doc(
                Get the first line of the TLE.

                Returns:
                    str: The first line of the TLE.

            )doc"
        )
        .def(
            "get_second_line",
            &TLE::getSecondLine,
            R"doc(
                Get the second line of the TLE.

                Returns:
                    str: The second line of the TLE.

            )doc"
        )
        .def(
            "get_satellite_number",
            &TLE::getSatelliteNumber,
            R"doc(
                Get the satellite number.

                Returns:
                    int: The satellite number.

            )doc"
        )
        .def(
            "get_classification",
            &TLE::getClassification,
            R"doc(
                Get the classification.

                Returns:
                    str: The classification.

            )doc"
        )
        .def(
            "get_international_designator",
            &TLE::getInternationalDesignator,
            R"doc(
                Get the international designator.

                Returns:
                    str: The international designator.

            )doc"
        )
        .def(
            "get_epoch",
            &TLE::getEpoch,
            R"doc(
                Get the epoch.

                Returns:
                    Instant: The epoch.

            )doc"
        )
        .def(
            "get_mean_motion_first_time_derivative_divided_by_two",
            &TLE::getMeanMotionFirstTimeDerivativeDividedByTwo,
            R"doc(
                Get the mean motion first time derivative divided by two.

                Returns:
                    float: The mean motion first time derivative divided by two.

            )doc"
        )
        .def(
            "get_mean_motion_second_time_derivative_divided_by_six",
            &TLE::getMeanMotionSecondTimeDerivativeDividedBySix,
            R"doc(
                Get the mean motion second time derivative divided by six.

                Returns:
                    float: The mean motion second time derivative divided by six.

            )doc"
        )
        .def(
            "get_b_star_drag_term",
            &TLE::getBStarDragTerm,
            R"doc(
                Get the B* drag term.

                Returns:
                    float: The B* drag term.

            )doc"
        )
        .def(
            "get_ephemeris_type",
            &TLE::getEphemerisType,
            R"doc(
                Get the ephemeris type.

                Returns:
                    int: The ephemeris type.

            )doc"
        )
        .def(
            "get_element_set_number",
            &TLE::getElementSetNumber,
            R"doc(
                Get the element set number.

                Returns:
                    int: The element set number.

            )doc"
        )
        .def(
            "get_first_line_checksum",
            &TLE::getFirstLineChecksum,
            R"doc(
                Get the checksum of the first line.

                Returns:
                    int: The checksum of the first line.

            )doc"
        )
        .def(
            "get_inclination",
            &TLE::getInclination,
            R"doc(
                Get the inclination.

                Returns:
                    Angle: The inclination.

            )doc"
        )
        .def(
            "get_raan",
            &TLE::getRaan,
            R"doc(
                Get the right ascension of the ascending node.

                Returns:
                    Angle: The right ascension of the ascending node.

            )doc"
        )
        .def(
            "get_eccentricity",
            &TLE::getEccentricity,
            R"doc(
                Get the eccentricity.

                Returns:
                    float: The eccentricity.

            )doc"
        )
        .def(
            "get_aop",
            &TLE::getAop,
            R"doc(
                Get the argument of perigee.

                Returns:
                    Angle: The argument of perigee.

            )doc"
        )
        .def(
            "get_mean_anomaly",
            &TLE::getMeanAnomaly,
            R"doc(
                Get the mean anomaly.

                Returns:
                    Angle: The mean anomaly.

            )doc"
        )
        .def(
            "get_mean_motion",
            &TLE::getMeanMotion,
            R"doc(
                Get the mean motion.

                Returns:
                    float: The mean motion.

            )doc"
        )
        .def(
            "get_revolution_number_at_epoch",
            &TLE::getRevolutionNumberAtEpoch,
            R"doc(
                Get the revolution number at epoch.

                Returns:
                    int: The revolution number at epoch.

            )doc"
        )
        .def(
            "get_second_line_checksum",
            &TLE::getSecondLineChecksum,
            R"doc(
                Get the checksum of the second line.

                Returns:
                    int: The checksum of the second line.

            )doc"
        )

        .def(
            "set_satellite_number",
            &TLE::setSatelliteNumber,
            arg("satellite_number"),
            R"doc(
                Set the satellite number.

                Args:
                    satellite_number (int): The satellite number.

            )doc"
        )

        .def(
            "set_epoch",
            &TLE::setEpoch,
            arg("epoch"),
            R"doc(
                Set the epoch.

                Args:
                    epoch (Instant): The epoch.

            )doc"
        )

        .def(
            "set_revolution_number_at_epoch",
            &TLE::setRevolutionNumberAtEpoch,
            arg("revolution_number"),
            R"doc(
                Set the revolution number at epoch.

                Args:
                    revolution_number (int): The revolution number at epoch.

            )doc"
        )

        .def_static(
            "undefined",
            &TLE::Undefined,
            R"doc(
                Create an undefined `TLE` object.

                Returns:
                    TLE: The undefined `TLE` object.
            )doc"
        )

        .def_static(
            "can_parse",
            +[](const String& aFirstLine, const String& aSecondLine) -> bool
            {
                return TLE::CanParse(aFirstLine, aSecondLine);
            },
            arg("first_line"),
            arg("second_line"),
            R"doc(
                Check if a TLE can be parsed from two strings.

                Args:
                    first_line (str): The first line of the TLE.
                    second_line (str): The second line of the TLE.

                Returns:
                    bool: True if the TLE can be parsed, False otherwise.
            )doc"
        )

        .def_static(
            "parse",
            &TLE::Parse,
            arg("string"),
            R"doc(
                Parse a TLE from a string.

                Args:
                    string (str): The string to parse.

                Returns:
                    TLE: The parsed TLE.
            )doc"
        )

        .def_static(
            "load",
            &TLE::Load,
            arg("file"),
            R"doc(
                Load a TLE from a file.

                Args:
                    file (str): The path to the file.

                Returns:
                    TLE: The loaded TLE.
            )doc"
        )

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
            arg("revolution_number_at_epoch"),
            R"doc(
                Construct a TLE.

                Args:
                    satellite_name (str): The name of the satellite.
                    satellite_number (int): The satellite number.
                    classification (str): The classification.
                    international_designator (str): The international designator.
                    epoch (Instant): The epoch.
                    mean_motion_first_time_derivative_divided_by_two (float): The mean motion first time derivative divided by two.
                    mean_motion_second_time_derivative_divided_by_six (float): The mean motion second time derivative divided by six.
                    b_star_drag_term (float): The B* drag term.
                    ephemeris_type (int): The ephemeris type.
                    element_set_number (int): The element set number.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    eccentricity (float): The eccentricity.
                    aop (Angle): The argument of perigee.
                    mean_anomaly (Angle): The mean anomaly.
                    mean_motion (float): The mean motion.
                    revolution_number_at_epoch (int): The revolution number at epoch.

                Returns:
                    TLE: The constructed TLE.
            )doc"
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
            arg("revolution_number_at_epoch"),
            R"doc(
                Construct a TLE.

                Args:
                    satellite_number (int): The satellite number.
                    classification (str): The classification.
                    international_designator (str): The international designator.
                    epoch (Instant): The epoch.
                    mean_motion_first_time_derivative_divided_by_two (float): The mean motion first time derivative divided by two.
                    mean_motion_second_time_derivative_divided_by_six (float): The mean motion second time derivative divided by six.
                    b_star_drag_term (float): The B* drag term.
                    ephemeris_type (int): The ephemeris type.
                    element_set_number (int): The element set number.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    eccentricity (float): The eccentricity.
                    aop (Angle): The argument of perigee.
                    mean_anomaly (Angle): The mean anomaly.
                    mean_motion (float): The mean motion.
                    revolution_number_at_epoch (int): The revolution number at epoch.

                Returns:
                    TLE: The constructed TLE.
            )doc"
        )

        .def_static(
            "generate_checksum",
            &TLE::GenerateChecksum,
            arg("string"),
            R"doc(
                Generate the checksum of a string.

                Args:
                    string (str): The string.

                Returns:
                    int: The checksum.
            )doc"
        )

        ;
}
