/// Apache License 2.0

#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_TLESolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
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
            init<const LeastSquaresSolver&, int, const String&, int, const bool>(),
            arg_v("solver", LeastSquaresSolver::Default(), "LeastSquaresSolver.default()"),
            arg("satellite_number") = 0,
            arg("international_designator") = "00001A",
            arg("revolution_number") = 0,
            arg("estimate_b_star") = true,
            R"doc(
                Construct a new TLESolver object.

                Args:
                    solver (LeastSquaresSolver, optional): The solver to use. Defaults to LeastSquaresSolver.default().
                    satellite_number (int, optional): Satellite number for TLE. Defaults to 0.
                    international_designator (str, optional): International designator for TLE. Defaults to "00001A".
                    revolution_number (int, optional): Revolution number. Defaults to 0.
                    estimate_b_star (bool, optional): Whether to also estimate the B* parameter. Defaults to True.
            )doc"
        )
        .def(
            "get_solver",
            &TLESolver::getSolver,
            return_value_policy::reference_internal,
            R"doc(
                Get the solver.

                Returns:
                    LeastSquaresSolver: The Least Squares solver.
            )doc"
        )
        .def(
            "get_satellite_number",
            &TLESolver::getSatelliteNumber,
            return_value_policy::reference_internal,
            R"doc(
                Get the satellite number.

                Returns:
                    int: The satellite number.
            )doc"
        )
        .def(
            "get_international_designator",
            &TLESolver::getInternationalDesignator,
            return_value_policy::reference_internal,
            R"doc(
                Get the international designator.

                Returns:
                    str: The international designator.
            )doc"
        )
        .def(
            "get_revolution_number",
            &TLESolver::getRevolutionNumber,
            return_value_policy::reference_internal,
            R"doc(
                Get the revolution number.

                Returns:
                    int: The revolution number.
            )doc"
        )
        .def(
            "get_default_b_star",
            &TLESolver::getDefaultBStar,
            return_value_policy::reference_internal,
            R"doc(
                Get the default B* value.

                Returns:
                    float: The default B* value.
            )doc"
        )
        .def(
            "get_first_derivative_mean_motion_divided_by_2",
            &TLESolver::getFirstDerivativeMeanMotionDividedBy2,
            return_value_policy::reference_internal,
            R"doc(
                Get the first derivative of mean motion divided by 2.

                Returns:
                    float: The first derivative of mean motion divided by 2.
            )doc"
        )
        .def(
            "get_second_derivative_mean_motion_divided_by_6",
            &TLESolver::getSecondDerivativeMeanMotionDividedBy6,
            return_value_policy::reference_internal,
            R"doc(
                Get the second derivative of mean motion divided by 6.

                Returns:
                    float: The second derivative of mean motion divided by 6.
            )doc"
        )
        .def(
            "get_ephemeris_type",
            &TLESolver::getEphemerisType,
            return_value_policy::reference_internal,
            R"doc(
                Get the ephemeris type.

                Returns:
                    int: The ephemeris type.
            )doc"
        )
        .def(
            "get_element_set_number",
            &TLESolver::getElementSetNumber,
            return_value_policy::reference_internal,
            R"doc(
                Get the element set number.

                Returns:
                    int: The element set number.
            )doc"
        )
        .def(
            "get_estimate_b_star",
            &TLESolver::getEstimateBStar,
            return_value_policy::reference_internal,
            R"doc(
                Get whether to estimate B*.

                Returns:
                    bool: whether to estimate B*.
            )doc"
        )
        .def(
            "get_tle_state_builder",
            &TLESolver::getTLEStateBuilder,
            return_value_policy::reference_internal,
            R"doc(
                Get the TLE state builder.

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
                std::variant<TLE, Pair<State, double>, State> cppInitialGuess = State::Undefined();

                if (isinstance<TLE>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<TLE>();
                }
                else if (isinstance<tuple>(anInitialGuess) && len(anInitialGuess.cast<tuple>()) == 2)
                {
                    auto t = anInitialGuess.cast<tuple>();
                    cppInitialGuess = Pair<State, double>(t[0].cast<State>(), t[1].cast<double>());
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
                std::variant<TLE, Pair<State, double>, State> cppInitialGuess = State::Undefined();

                if (isinstance<TLE>(anInitialGuess))
                {
                    cppInitialGuess = anInitialGuess.cast<TLE>();
                }
                else if (isinstance<tuple>(anInitialGuess) && len(anInitialGuess.cast<tuple>()) == 2)
                {
                    auto t = anInitialGuess.cast<tuple>();
                    cppInitialGuess = Pair<State, double>(t[0].cast<State>(), t[1].cast<double>());
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
