/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ODLeastSquaresSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_ODLeastSquaresSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::Environment;
    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::estimator::ODLeastSquaresSolver;
    using ostk::astrodynamics::solver::LeastSquaresSolver;
    using ostk::astrodynamics::trajectory::NumericalSolver;
    using ostk::astrodynamics::trajectory::Orbit;
    using ostk::astrodynamics::trajectory::Propagator;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<ODLeastSquaresSolver> odLeastSquaresSolver(
        aModule,
        "ODLeastSquaresSolver",
        R"doc(
            Orbit Determination solver using Least Squares estimation.
        )doc"
    );

    class_<ODLeastSquaresSolver::Analysis>(
        odLeastSquaresSolver,
        "Analysis",
        R"doc(
            Analysis results from the Orbit Determination Least Squares Solver.
        )doc"
    )
        .def(
            init<const State&, const LeastSquaresSolver::Analysis&>(),
            arg("determined_state"),
            arg("solver_analysis"),
            R"doc(
                Construct a new Analysis object.

                Args:
                    determined_state (State): The determined state.
                    solver_analysis (LeastSquaresSolverAnalysis): The solver analysis.
            )doc"
        )
        .def("__str__", &(shiftToString<ODLeastSquaresSolver::Analysis>))
        .def("__repr__", &(shiftToString<ODLeastSquaresSolver::Analysis>))
        .def_readonly(
            "determined_state",
            &ODLeastSquaresSolver::Analysis::determinedState,
            R"doc(
                The determined state.

                Returns:
                    State: The determined state.
            )doc"
        )
        .def_readonly(
            "solver_analysis",
            &ODLeastSquaresSolver::Analysis::solverAnalysis,
            R"doc(
                The solver analysis.

                Returns:
                    LeastSquaresSolverAnalysis: The solver analysis.
            )doc"
        )

        ;

    odLeastSquaresSolver
        .def(
            init<const Environment&, const NumericalSolver&, const LeastSquaresSolver&, const Shared<Frame>&>(),
            arg_v("environment", DEFAULT_ENVIRONMENT, "Environment.default()"),
            arg_v("numerical_solver", DEFAULT_NUMERICAL_SOLVER, "NumericalSolver.default()"),
            arg_v("solver", DEFAULT_LEAST_SQUARES_SOLVER, "LeastSquaresSolver.default()"),
            arg_v("estimation_frame", DEFAULT_ESTIMATION_FRAME, "Frame.GCRF()"),
            R"doc(
                Construct a new ODLeastSquaresSolver object.

                Args:
                    environment (Environment, optional): The environment. Defaults to Environment.default().
                    numerical_solver (NumericalSolver, optional): The numerical solver. Defaults to NumericalSolver.default().
                    solver (LeastSquaresSolver, optional): The Least Squares solver. Defaults to LeastSquaresSolver.default().
                    estimation_frame (Frame, optional): The estimation frame. Defaults to Frame.GCRF().
            )doc"
        )
        .def(
            "access_environment",
            &ODLeastSquaresSolver::accessEnvironment,
            return_value_policy::reference_internal,
            R"doc(
                Access the environment.

                Returns:
                    Environment: The environment.
            )doc"
        )
        .def(
            "access_propagator",
            &ODLeastSquaresSolver::accessPropagator,
            return_value_policy::reference_internal,
            R"doc(
                Access the propagator.

                Returns:
                    Propagator: The propagator.
            )doc"
        )
        .def(
            "access_solver",
            &ODLeastSquaresSolver::accessSolver,
            return_value_policy::reference_internal,
            R"doc(
                Access the Least Squares solver.

                Returns:
                    LeastSquaresSolver: The Least Squares solver.
            )doc"
        )
        .def(
            "estimate_state",
            &ODLeastSquaresSolver::estimateState,
            arg("initial_guess_state"),
            arg("observations"),
            arg_v("estimation_coordinate_subsets", Array<Shared<const CoordinateSubset>>::Empty(), "[]"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate state using Least Squares.

                Args:
                    initial_guess_state (State): Initial guess state.
                    observations (list[State]): Observations to fit against.
                    estimation_coordinate_subsets (list[CoordinateSubset], optional): Coordinate subsets to estimate. Defaults to empty list, in which case all the coordinate subsets from the initial guess state are estimated.
                    initial_guess_sigmas (dict[CoordinateSubset, VectorXd], optional): Initial guess sigmas.
                    observation_sigmas (dict[CoordinateSubset, VectorXd], optional): Observation sigmas.

                Returns:
                    ODLeastSquaresSolverAnalysis: The analysis results.
            )doc"
        )
        .def(
            "estimate_orbit",
            &ODLeastSquaresSolver::estimateOrbit,
            arg("initial_guess_state"),
            arg("observations"),
            arg_v("estimation_coordinate_subsets", Array<Shared<const CoordinateSubset>>::Empty(), "[]"),
            arg_v("initial_guess_sigmas", DEFAULT_INITIAL_GUESS_SIGMAS, "{}"),
            arg_v("observation_sigmas", DEFAULT_OBSERVATION_SIGMAS, "{}"),
            R"doc(
                Estimate orbit using Least Squares.

                Args:
                    initial_guess_state (State): Initial guess state.
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
