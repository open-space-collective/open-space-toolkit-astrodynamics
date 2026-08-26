/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_OrbitDeterminationSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::Environment;

    using ostk::astrodynamics::estimator::ObservationFilter;
    using ostk::astrodynamics::estimator::OrbitDeterminationSolver;
    using ostk::astrodynamics::solver::LeastSquaresSolver;
    using ostk::astrodynamics::trajectory::NumericalSolver;
    using ostk::astrodynamics::trajectory::Orbit;
    using ostk::astrodynamics::trajectory::Propagator;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<OrbitDeterminationSolver> orbitDeterminationSolver(
        aModule,
        "OrbitDeterminationSolver",
        R"doc(
            Orbit Determination solver.
        )doc"
    );

    class_<OrbitDeterminationSolver::Analysis>(
        orbitDeterminationSolver,
        "Analysis",
        R"doc(
            Analysis results from the Orbit Determination.
        )doc"
    )
        .def(
            init<const State&, const LeastSquaresSolver::Analysis&, const Shared<const ObservationFilter::Analysis>&>(),
            arg("estimated_state"),
            arg("solver_analysis"),
            arg_v("observation_filter_analysis", nullptr, "None"),
            R"doc(
                Construct a new Analysis object.

                Args:
                    estimated_state (State): The estimated state. Matching the frame and expanded coordinates of the provided initial guess state.
                    solver_analysis (LeastSquaresSolver.Analysis): The solver analysis.
                    observation_filter_analysis (ObservationFilter.Analysis, optional): The observation filter analysis, when an observation filter was applied. Defaults to None.
            )doc"
        )
        .def("__str__", &(shiftToString<OrbitDeterminationSolver::Analysis>))
        .def("__repr__", &(shiftToString<OrbitDeterminationSolver::Analysis>))
        .def_readonly(
            "estimated_state",
            &OrbitDeterminationSolver::Analysis::estimatedState,
            R"doc(
                The estimated state.

                :type: State
            )doc"
        )
        .def_readonly(
            "solver_analysis",
            &OrbitDeterminationSolver::Analysis::solverAnalysis,
            R"doc(
                The solver analysis.

                :type: LeastSquaresSolver.Analysis
            )doc"
        )
        .def_readonly(
            "observation_filter_analysis",
            &OrbitDeterminationSolver::Analysis::observationFilterAnalysis,
            R"doc(
                The observation filter analysis (None when no observation filter was applied).

                :type: ObservationFilter.Analysis
            )doc"
        )

        ;

    orbitDeterminationSolver
        .def(
            init<
                const Environment&,
                const NumericalSolver&,
                const LeastSquaresSolver&,
                const Shared<Frame>&,
                const Shared<const ObservationFilter>&>(),
            arg_v("environment", DEFAULT_ENVIRONMENT, "Environment.default()"),
            arg_v("numerical_solver", DEFAULT_NUMERICAL_SOLVER, "NumericalSolver.default()"),
            arg_v("solver", DEFAULT_LEAST_SQUARES_SOLVER, "LeastSquaresSolver.default()"),
            arg_v("estimation_frame", DEFAULT_ESTIMATION_FRAME, "Frame.GCRF()"),
            arg_v("observation_filter", nullptr, "None"),
            R"doc(
                Construct a new OrbitDeterminationSolver object.

                Args:
                    environment (Environment, optional): The environment. Defaults to Environment.default().
                    numerical_solver (NumericalSolver, optional): The numerical solver. Defaults to NumericalSolver.default().
                    solver (LeastSquaresSolver, optional): The Least Squares solver. Defaults to LeastSquaresSolver.default().
                    estimation_frame (Frame, optional): The estimation frame. Defaults to Frame.GCRF().
                    observation_filter (ObservationFilter, optional): Observation filter applied to the observations (in the estimation frame) before solving, as a pre-filtering step rejecting outlier observations. Defaults to None (no filtering).
            )doc"
        )
        .def(
            "access_environment",
            &OrbitDeterminationSolver::accessEnvironment,
            return_value_policy::reference_internal,
            R"doc(
                Access the environment.

                Returns:
                    Environment: The environment.
            )doc"
        )
        .def(
            "access_propagator",
            &OrbitDeterminationSolver::accessPropagator,
            return_value_policy::reference_internal,
            R"doc(
                Access the propagator.

                Returns:
                    Propagator: The propagator.
            )doc"
        )
        .def(
            "access_solver",
            &OrbitDeterminationSolver::accessSolver,
            return_value_policy::reference_internal,
            R"doc(
                Access the solver.

                Returns:
                    LeastSquaresSolver: The Least Squares solver.
            )doc"
        )
        .def(
            "access_estimation_frame",
            &OrbitDeterminationSolver::accessEstimationFrame,
            return_value_policy::reference_internal,
            R"doc(
                Access the estimation frame.

                Returns:
                    Frame: The estimation frame.
            )doc"
        )
        .def(
            "access_observation_filter",
            &OrbitDeterminationSolver::accessObservationFilter,
            return_value_policy::reference_internal,
            R"doc(
                Access the observation filter.

                Returns:
                    ObservationFilter: The observation filter (None when no filter is configured).
            )doc"
        )
        .def(
            "estimate",
            &OrbitDeterminationSolver::estimate,
            arg("initial_guess"),
            arg("observations"),
            arg_v("estimation_coordinate_subsets", Array<Shared<const CoordinateSubset>>::Empty(), "[]"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate state from observations.

                Args:
                    initial_guess (State): Initial guess state.
                    observations (list[State]): Observations to fit against.
                    estimation_coordinate_subsets (list[CoordinateSubset], optional): Coordinate subsets to estimate. Defaults to empty list, in which case all the coordinate subsets from the initial guess state are estimated.
                    initial_guess_sigmas (dict[CoordinateSubset, VectorXd], optional): Initial guess sigmas.
                    observation_sigmas (dict[CoordinateSubset, VectorXd], optional): Observation sigmas.

                Returns:
                    OrbitDeterminationSolverAnalysis: The analysis results.
            )doc"
        )
        .def(
            "estimate_orbit",
            &OrbitDeterminationSolver::estimateOrbit,
            arg("initial_guess"),
            arg("observations"),
            arg_v("estimation_coordinate_subsets", Array<Shared<const CoordinateSubset>>::Empty(), "[]"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate Propagated Orbit from observations.

                Args:
                    initial_guess (State): Initial guess state.
                    observations (list[State]): Observations to fit against.
                    estimation_coordinate_subsets (list[CoordinateSubset], optional): Coordinate subsets to estimate. Defaults to empty list, in which case all the coordinate subsets from the initial guess state are estimated.
                    initial_guess_sigmas (dict[CoordinateSubset, VectorXd], optional): Initial guess sigmas. Defaults to empty, in which case
                    observation_sigmas (dict[CoordinateSubset, VectorXd], optional): Observation sigmas.

                Returns:
                    Orbit: The estimated orbit.
            )doc"
        )

        ;
}
