/// Apache License 2.0

#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_TLESolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::mathematics::object::VectorXd;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::estimator::TLESolver;
    using ostk::astrodynamics::solver::LeastSquaresSolver;
    using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<TLESolver> tleSolver(
        aModule,
        "TLESolver",
        R"doc(
            Solver for estimating TLE elements.
        )doc"
    )

        ;

    class_<TLESolver::Analysis>(
        tleSolver,
        "Analysis",
        R"doc(
            Analysis results from the TLE estimation solver.
        )doc"
    )
        .def(
            init<const TLE&, const LeastSquaresSolver::Analysis&>(),
            arg("estimated_tle"),
            arg("solver_analysis"),
            R"doc(
                Construct a new TLESolver::Analysis object.

                Args:
                    estimated_tle (TLE): The estimated TLE.
                    solver_analysis (LeastSquaresSolver.Analysis): The solver analysis.
            )doc"
        )
        .def("__str__", &(shiftToString<TLESolver::Analysis>))
        .def("__repr__", &(shiftToString<TLESolver::Analysis>))
        .def_readonly(
            "estimated_tle",
            &TLESolver::Analysis::estimatedTLE,
            R"doc(
                The estimated TLE.

                :type: TLE
            )doc"
        )
        .def_readonly(
            "solver_analysis",
            &TLESolver::Analysis::solverAnalysis,
            R"doc(
                The solver analysis.

                :type: LeastSquaresSolver.Analysis
            )doc"
        )

        ;

    tleSolver
        .def(
            init<
                const LeastSquaresSolver&,
                const Integer&,
                const String&,
                const Integer&,
                const bool,
                const Shared<const Frame>&>(),
            arg_v("solver", LeastSquaresSolver::Default(), "LeastSquaresSolver.default()"),
            arg("satellite_number") = 0,
            arg("international_designator") = "00001A",
            arg("revolution_number") = 0,
            arg("estimate_b_star") = true,
            arg_v("estimation_frame", Frame::GCRF(), "Frame.GCRF()"),
            R"doc(
                Construct a new TLESolver object.

                Args:
                    solver (LeastSquaresSolver, optional): The solver to use. Defaults to LeastSquaresSolver.default().
                    satellite_number (int, optional): Satellite number for TLE. Defaults to 0.
                    international_designator (str, optional): International designator for TLE. Defaults to "00001A".
                    revolution_number (int, optional): Revolution number. Defaults to 0.
                    estimate_b_star (bool, optional): Whether to also estimate the B* parameter. Defaults to True.
                    estimation_frame (Frame, optional): Frame for estimation. Defaults to GCRF.
            )doc"
        )
        .def(
            "access_solver",
            &TLESolver::accessSolver,
            return_value_policy::reference_internal,
            R"doc(
                Access the solver.

                Returns:
                    LeastSquaresSolver: The Least Squares solver.
            )doc"
        )
        .def(
            "access_satellite_number",
            &TLESolver::accessSatelliteNumber,
            return_value_policy::reference_internal,
            R"doc(
                Access the satellite number.

                Returns:
                    int: The satellite number.
            )doc"
        )
        .def(
            "access_international_designator",
            &TLESolver::accessInternationalDesignator,
            return_value_policy::reference_internal,
            R"doc(
                Access the international designator.

                Returns:
                    str: The international designator.
            )doc"
        )
        .def(
            "access_revolution_number",
            &TLESolver::accessRevolutionNumber,
            return_value_policy::reference_internal,
            R"doc(
                Access the revolution number.

                Returns:
                    int: The revolution number.
            )doc"
        )
        .def(
            "access_default_b_star",
            &TLESolver::accessDefaultBStar,
            return_value_policy::reference_internal,
            R"doc(
                Access the default B* value.

                Returns:
                    float: The default B* value.
            )doc"
        )
        .def(
            "access_first_derivative_mean_motion_divided_by_2",
            &TLESolver::accessFirstDerivativeMeanMotionDividedBy2,
            return_value_policy::reference_internal,
            R"doc(
                Access the first derivative of mean motion divided by 2.

                Returns:
                    float: The first derivative of mean motion divided by 2.
            )doc"
        )
        .def(
            "access_second_derivative_mean_motion_divided_by_6",
            &TLESolver::accessSecondDerivativeMeanMotionDividedBy6,
            return_value_policy::reference_internal,
            R"doc(
                Access the second derivative of mean motion divided by 6.

                Returns:
                    float: The second derivative of mean motion divided by 6.
            )doc"
        )
        .def(
            "access_ephemeris_type",
            &TLESolver::accessEphemerisType,
            return_value_policy::reference_internal,
            R"doc(
                Access the ephemeris type.

                Returns:
                    int: The ephemeris type.
            )doc"
        )
        .def(
            "access_element_set_number",
            &TLESolver::accessElementSetNumber,
            return_value_policy::reference_internal,
            R"doc(
                Access the element set number.

                Returns:
                    int: The element set number.
            )doc"
        )
        .def(
            "access_estimate_b_star",
            &TLESolver::accessEstimateBStar,
            return_value_policy::reference_internal,
            R"doc(
                Access whether to estimate B*.

                Returns:
                    bool: whether to estimate B*.
            )doc"
        )
        .def(
            "access_tle_state_builder",
            &TLESolver::accessTLEStateBuilder,
            return_value_policy::reference_internal,
            R"doc(
                Access the TLE state builder.

                Returns:
                    StateBuilder: The TLE state builder.
            )doc"
        )
        .def(
            "estimate",
            [](const TLESolver& self,
               const object& anInitialGuess,
               const Array<State>& anObservationStateArray,
               const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
               const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas)
            {
                std::variant<TLE, Pair<State, Real>, State> cppInitialGuess = State::Undefined();

                if (isinstance<TLE>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<TLE>();
                }
                else if (isinstance<tuple>(anInitialGuess) && len(anInitialGuess.cast<tuple>()) == 2)
                {
                    auto t = anInitialGuess.cast<tuple>();
                    cppInitialGuess = Pair<State, Real>(t[0].cast<State>(), t[1].cast<Real>());
                }
                else if (isinstance<State>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<State>();
                }
                else
                {
                    throw std::runtime_error("Initial guess must be a TLE, tuple[State, float], or State.");
                }

                return self.estimate(
                    cppInitialGuess, anObservationStateArray, anInitialGuessSigmas, anObservationSigmas
                );
            },
            arg("initial_guess"),
            arg("observations"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate TLE from observations.

                Args:
                    initial_guess (TLE | tuple[State, float] | State): Initial guess - can be a TLE, (cartesian State, B*) tuple, or cartesian State.
                    observations (list[State]): State observations to fit against.
                    initial_guess_sigmas (dict[CoordinateSubset, ndarray], optional): Initial guess sigmas.
                    observation_sigmas (dict[CoordinateSubset, ndarray], optional): Observation sigmas.

                Returns:
                    TLESolver.Analysis: Analysis results containing the estimated TLE and solver analysis.
            )doc"
        )
        .def(
            "estimate_orbit",
            [](const TLESolver& self,
               const object& anInitialGuess,
               const Array<State>& anObservationStateArray,
               const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
               const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas)
            {
                std::variant<TLE, Pair<State, Real>, State> cppInitialGuess = State::Undefined();

                if (isinstance<TLE>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<TLE>();
                }
                else if (isinstance<tuple>(anInitialGuess) && len(anInitialGuess.cast<tuple>()) == 2)
                {
                    auto t = anInitialGuess.cast<tuple>();
                    cppInitialGuess = Pair<State, Real>(t[0].cast<State>(), t[1].cast<Real>());
                }
                else if (isinstance<State>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<State>();
                }
                else
                {
                    throw std::runtime_error("Initial guess must be a TLE, tuple[State, float], or State.");
                }

                return self.estimateOrbit(
                    cppInitialGuess, anObservationStateArray, anInitialGuessSigmas, anObservationSigmas
                );
            },
            arg("initial_guess"),
            arg("observations"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate an SGP4-based orbit from observations.

                Args:
                    initial_guess (TLE | tuple[State, float] | State): Initial guess - can be a TLE, (cartesian State, B*) tuple, or cartesian State.
                    observations (list[State]): State observations to fit against.
                    initial_guess_sigmas (dict[CoordinateSubset, ndarray], optional): Initial guess sigmas.
                    observation_sigmas (dict[CoordinateSubset, ndarray], optional): Observation sigmas.

                Returns:
                    Orbit: The estimated SGP4 orbit.
            )doc"
        )

        ;
}
