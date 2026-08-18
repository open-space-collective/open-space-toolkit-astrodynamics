/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/MeanElements.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4_MeanElements(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;
    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::trajectory::orbit::model::sgp4::MeanElements;
    using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
    using ostk::astrodynamics::trajectory::State;

    class_<MeanElements>(
        aModule,
        "MeanElements",
        R"doc(
            SGP4 mean element set, held at full precision, and the propagator that flies it.

            Reach for this when the elements are being computed rather than read: fitted by an
            estimator, differentiated, stepped, or produced by anything other than 69 columns of
            text. It takes the eight elements as continuous values and imposes no quantum on any of
            them. To propagate a TLE that was published, received, or read from a file, use `SGP4`.

            A TLE is a lossy serialization of this set: writing one rounds the eccentricity to 1e-7,
            the angles to 1e-4 deg, the mean motion to 1e-8 rev/day and the epoch to 1e-8 day. That
            rounding is harmless for propagation but fatal for differentiation — on a near-circular
            orbit a finite-difference step in eccentricity can be smaller than the quantum, so the
            propagator does not respond at all and the Jacobian silently loses rank. Estimators
            should iterate on `MeanElements` and serialize a TLE once, at the end, which costs a few
            tens of millimetres.

            Underneath there is one propagator: `SGP4` decodes its TLE into a `MeanElements` and
            propagates that. Fed the same values the two agree exactly; they differ only in what
            precision can reach them.

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
                const Real&>(),
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
                    b_star_drag_term (float): The B* drag term. Defaults to 0.0.

            )doc",
            arg("epoch"),
            arg("inclination"),
            arg("raan"),
            arg("eccentricity"),
            arg("aop"),
            arg("mean_anomaly"),
            arg("mean_motion"),
            arg_v("b_star_drag_term", Real(0.0), "0.0")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<MeanElements>))
        .def("__repr__", &(shiftToString<MeanElements>))

        .def(
            "is_defined",
            &MeanElements::isDefined,
            R"doc(
                Check if the mean element set is defined.

                Returns:
                    bool: True if the mean element set is defined, False otherwise.

            )doc"
        )

        .def(
            "get_epoch",
            &MeanElements::getEpoch,
            R"doc(
                Get the epoch.

                Returns:
                    Instant: The epoch.

            )doc"
        )

        .def(
            "get_inclination",
            &MeanElements::getInclination,
            R"doc(
                Get the inclination.

                Returns:
                    Angle: The inclination.

            )doc"
        )

        .def(
            "get_raan",
            &MeanElements::getRaan,
            R"doc(
                Get the right ascension of the ascending node.

                Returns:
                    Angle: The right ascension of the ascending node.

            )doc"
        )

        .def(
            "get_eccentricity",
            &MeanElements::getEccentricity,
            R"doc(
                Get the eccentricity.

                Returns:
                    float: The eccentricity.

            )doc"
        )

        .def(
            "get_aop",
            &MeanElements::getAop,
            R"doc(
                Get the argument of periapsis.

                Returns:
                    Angle: The argument of periapsis.

            )doc"
        )

        .def(
            "get_mean_anomaly",
            &MeanElements::getMeanAnomaly,
            R"doc(
                Get the mean anomaly.

                Returns:
                    Angle: The mean anomaly.

            )doc"
        )

        .def(
            "get_mean_motion",
            &MeanElements::getMeanMotion,
            R"doc(
                Get the mean motion.

                Returns:
                    Derived: The mean motion.

            )doc"
        )

        .def(
            "get_b_star_drag_term",
            &MeanElements::getBStarDragTerm,
            R"doc(
                Get the B* drag term.

                Returns:
                    float: The B* drag term.

            )doc"
        )

        .def(
            "calculate_state_at",
            &MeanElements::calculateStateAt,
            arg("instant"),
            arg_v("output_frame", Frame::TEME(), "Frame.TEME()"),
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.
                    output_frame (Frame): The output frame. Defaults to TEME.

                Returns:
                    State: The state.

            )doc"
        )

        .def(
            "calculate_states_at",
            &MeanElements::calculateStatesAt,
            arg("instants"),
            arg_v("output_frame", Frame::TEME(), "Frame.TEME()"),
            R"doc(
                Calculate the states at given instants.

                Args:
                    instants (list[Instant]): The instants.
                    output_frame (Frame): The output frame. Defaults to TEME.

                Returns:
                    list[State]: The states.

            )doc"
        )

        .def_static(
            "undefined",
            &MeanElements::Undefined,
            R"doc(
                Construct an undefined mean element set.

                Returns:
                    MeanElements: An undefined mean element set.

            )doc"
        )

        .def_static(
            "from_tle",
            &MeanElements::FromTLE,
            arg("tle"),
            R"doc(
                Construct a mean element set from a TLE.

                The values carried by the TLE have already been rounded by its text format; this
                does not recover the precision that was lost when the TLE was written.

                Args:
                    tle (TLE): The TLE.

                Returns:
                    MeanElements: The mean element set the TLE encodes.

            )doc"
        )

        ;
}
