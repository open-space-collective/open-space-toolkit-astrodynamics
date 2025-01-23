/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ODLeastSquaresSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_ODLeastSquaresSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::Environment;

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
            Orbit Determination solver using least squares estimation.
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
        .def(
            "get_determined_state",
            &ODLeastSquaresSolver::Analysis::getDeterminedState,
            R"doc(
                Get the determined state.

                Returns:
                    State: The determined state.
            )doc"

        )
        .def(
            "get_solver_analysis",
            &ODLeastSquaresSolver::Analysis::getSolverAnalysis,
            R"doc(
                Get the solver analysis.

                Returns:
                    LeastSquaresSolverAnalysis: The solver analysis.
            )doc"
        )

        ;

    odLeastSquaresSolver
        .def(
            init<const Environment&, const NumericalSolver&, const LeastSquaresSolver&>(),
            arg("environment"),
            arg("numerical_solver") = NumericalSolver::Default(),
            arg("solver") = LeastSquaresSolver::Default(),
            R"doc(
                Construct a new ODLeastSquaresSolver object.

                Args:
                    environment (Environment): The environment.
                    numerical_solver (NumericalSolver, optional): The numerical solver. Defaults to NumericalSolver.default().
                    solver (LeastSquaresSolver, optional): The least squares solver. Defaults to LeastSquaresSolver.default().
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
                Access the least squares solver.

                Returns:
                    LeastSquaresSolver: The least squares solver.
            )doc"
        )
        .def(
            "estimate_state",
            &ODLeastSquaresSolver::estimateState,
            arg("initial_guess_state"),
            arg("reference_states"),
            arg("estimation_coordinate_subsets") = Array<Shared<const CoordinateSubset>>::Empty(),
            arg("initial_guess_sigmas") = std::unordered_map<CoordinateSubset, VectorXd>(),
            arg("reference_state_sigmas") = std::unordered_map<CoordinateSubset, VectorXd>(),
            R"doc(
                Estimate state using least squares.

                Args:
                    initial_guess_state (State): Initial guess state.
                    reference_states (list[State]): Reference states to minimize error against.
                    estimation_coordinate_subsets (list[CoordinateSubset], optional): Coordinate subsets to estimate.
                    initial_guess_sigmas (dict[CoordinateSubset, VectorXd], optional): Initial guess sigmas.
                    reference_state_sigmas (dict[CoordinateSubset, VectorXd], optional): Reference state sigmas.

                Returns:
                    ODLeastSquaresSolverAnalysis: The analysis results.
            )doc"
        )
        .def(
            "estimate_orbit",
            &ODLeastSquaresSolver::estimateOrbit,
            arg("initial_guess_state"),
            arg("reference_states"),
            arg("estimation_coordinate_subsets") = Array<Shared<const CoordinateSubset>>::Empty(),
            arg("initial_guess_sigmas") = std::unordered_map<CoordinateSubset, VectorXd>(),
            arg("reference_state_sigmas") = std::unordered_map<CoordinateSubset, VectorXd>(),
            R"doc(
                Estimate orbit using least squares.

                Args:
                    initial_guess_state (State): Initial guess state.
                    reference_states (list[State]): Reference states to minimize error against.
                    estimation_coordinate_subsets (list[CoordinateSubset], optional): Coordinate subsets to estimate.
                    initial_guess_sigmas (dict[CoordinateSubset, VectorXd], optional): Initial guess sigmas.
                    reference_state_sigmas (dict[CoordinateSubset, VectorXd], optional): Reference state sigmas.

                Returns:
                    Orbit: The estimated orbit.
            )doc"
        )

        ;
}
