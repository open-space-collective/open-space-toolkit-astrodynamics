/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/SGP4FullPrecision.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;
    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::trajectory::orbit::model::sgp4::SGP4FullPrecision;
    using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
    using ostk::astrodynamics::trajectory::State;

    class_<SGP4FullPrecision, ostk::astrodynamics::trajectory::orbit::Model>(
        aModule,
        "SGP4FullPrecision",
        R"doc(
            An Orbit Model that holds SGP4 mean elements at full precision.
            This uses the SGP4 propagator, but with the orbital elements stored at machine-precision rather than the truncated precision of the TLE format.
        )doc"
    )

        .def(
            init<
                const Instant&,
                const Angle&,
                const Angle&,
                const Real&,
                const Angle&,
                const Angle&,
                const Derived&,
                const Real&,
                const Integer&,
                const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Args:
                    epoch (Instant): The epoch.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    eccentricity (float): The eccentricity.
                    aop (Angle): The argument of periapsis.
                    mean_anomaly (Angle): The mean anomaly.
                    mean_motion (Derived): The mean motion.
                    b_star_drag_term (float): The B* drag term.
                    revolution_number_at_epoch (int): The revolution number at epoch. Defaults to 1.
                    output_frame (Frame): The output frame. Defaults to TEME.

            )doc",
            arg("epoch"),
            arg("inclination"),
            arg("raan"),
            arg("eccentricity"),
            arg("aop"),
            arg("mean_anomaly"),
            arg("mean_motion"),
            arg("b_star_drag_term"),
            arg_v("revolution_number_at_epoch", Integer(1), "1"),
            arg_v("output_frame", Frame::TEME(), "Frame.TEME()")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<SGP4FullPrecision>))
        .def("__repr__", &(shiftToString<SGP4FullPrecision>))

        .def(
            "is_defined",
            &SGP4FullPrecision::isDefined,
            R"doc(
                Check if the mean element set is defined.

                Returns:
                    bool: True if the mean element set is defined, False otherwise.

            )doc"
        )

        .def(
            "get_epoch",
            &SGP4FullPrecision::getEpoch,
            R"doc(
                Get the epoch.

                Returns:
                    Instant: The epoch.

            )doc"
        )

        .def(
            "get_inclination",
            &SGP4FullPrecision::getInclination,
            R"doc(
                Get the inclination.

                Returns:
                    Angle: The inclination.

            )doc"
        )

        .def(
            "get_raan",
            &SGP4FullPrecision::getRaan,
            R"doc(
                Get the right ascension of the ascending node.

                Returns:
                    Angle: The right ascension of the ascending node.

            )doc"
        )

        .def(
            "get_eccentricity",
            &SGP4FullPrecision::getEccentricity,
            R"doc(
                Get the eccentricity.

                Returns:
                    float: The eccentricity.

            )doc"
        )

        .def(
            "get_aop",
            &SGP4FullPrecision::getAop,
            R"doc(
                Get the argument of periapsis.

                Returns:
                    Angle: The argument of periapsis.

            )doc"
        )

        .def(
            "get_mean_anomaly",
            &SGP4FullPrecision::getMeanAnomaly,
            R"doc(
                Get the mean anomaly.

                Returns:
                    Angle: The mean anomaly.

            )doc"
        )

        .def(
            "get_mean_motion",
            &SGP4FullPrecision::getMeanMotion,
            R"doc(
                Get the mean motion.

                Returns:
                    Derived: The mean motion.

            )doc"
        )

        .def(
            "get_b_star_drag_term",
            &SGP4FullPrecision::getBStarDragTerm,
            R"doc(
                Get the B* drag term.

                Returns:
                    float: The B* drag term.

            )doc"
        )

        .def(
            "get_revolution_number_at_epoch",
            &SGP4FullPrecision::getRevolutionNumberAtEpoch,
            R"doc(
                Get the revolution number at epoch.

                Returns:
                    int: The revolution number at epoch.

            )doc"
        )

        .def(
            "get_output_frame",
            &SGP4FullPrecision::getOutputFrame,
            R"doc(
                Get the output frame.

                Returns:
                    Frame: The output frame.

            )doc"
        )

        .def(
            "calculate_state_at",
            &SGP4FullPrecision::calculateStateAt,
            arg("instant"),
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state, expressed in the output frame.

            )doc"
        )

        .def(
            "calculate_states_at",
            &SGP4FullPrecision::calculateStatesAt,
            arg("instants"),
            R"doc(
                Calculate the states at given instants.

                Args:
                    instants (list[Instant]): The instants.

                Returns:
                    list[State]: The states, expressed in the output frame.

            )doc"
        )

        .def_static(
            "undefined",
            &SGP4FullPrecision::Undefined,
            R"doc(
                Construct an undefined mean element set.

                Returns:
                    SGP4FullPrecision: An undefined mean element set.

            )doc"
        )

        .def_static(
            "from_tle",
            &SGP4FullPrecision::FromTLE,
            arg("tle"),
            arg_v("output_frame", Frame::TEME(), "Frame.TEME()"),
            R"doc(
                Construct a mean element set from a TLE.

                The values carried by the TLE have already been rounded by its text format; this
                does not recover the precision that was lost when the TLE was written.

                Args:
                    tle (TLE): The TLE.
                    output_frame (Frame): The output frame. Defaults to TEME.

                Returns:
                    SGP4FullPrecision: The mean element set the TLE encodes.

            )doc"
        )

        ;
}
