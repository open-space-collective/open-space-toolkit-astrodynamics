
/// Apache License 2.0

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>

namespace py = pybind11;

inline void OpenSpaceToolkitAstrodynamicsPy_Solver_LeastSquaresSolver(py::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
    using ostk::core::type::Real;
    using ostk::core::type::Size;
    using ostk::core::type::String;

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::VectorXd;

    using ostk::physics::time::Instant;

    using ostk::astrodynamics::solver::LeastSquaresSolver;
    using ostk::astrodynamics::trajectory::State;

    class_<LeastSquaresSolver> leastSquaresSolver(
        aModule,
        "LeastSquaresSolver",
        R"doc(
            Class to solve non-linear least squares problems.
        )doc"
    );

    class_<LeastSquaresSolver::Step>(
        leastSquaresSolver,
        "Step",
        R"doc(
            Class representing a step in the least squares solver.
        )doc"
    )
        .def(
            init<const Real&, const VectorXd&>(),
            R"doc(
                Constructor.

                Args:
                    aRmsError (float): The RMS error.
                    anXHat (np.ndarray): The X hat vector.
            )doc",
            arg("aRmsError"),
            arg("anXHat")
        )
        .def("__str__", &(shiftToString<LeastSquaresSolver::Step>))
        .def("__repr__", &(shiftToString<LeastSquaresSolver::Step>))
        .def_readonly(
            "rms_error",
            &LeastSquaresSolver::Step::rmsError,
            R"doc(
                The RMS error.

                :type: float
            )doc"
        )
        .def_readonly(
            "x_hat",
            &LeastSquaresSolver::Step::xHat,
            R"doc(
                The X hat vector.

                :type: np.ndarray
            )doc"
        )

        ;

    class_<LeastSquaresSolver::Analysis>(
        leastSquaresSolver,
        "Analysis",
        R"doc(
            Class representing the analysis of the least squares solver.
        )doc"
    )
        .def(
            init<
                const Real&,
                const Size&,
                const String&,
                const State&,
                const MatrixXd&,
                const MatrixXd&,
                const Array<LeastSquaresSolver::Step>&>(),
            R"doc(
                Constructor.

                Args:
                    rms_error (float): The RMS error.
                    iteration_count (int): The iteration count.
                    termination_criteria (str): The termination criteria.
                    solution_state (State): The solution state.
                    solution_covariance (np.ndarray): The solution covariance matrix.
                    solution_frisbee_covariance (np.ndarray): The solution Frisbee covariance matrix.
                    steps (list[LeastSquaresSolver.Step]): The steps.
            )doc",
            arg("rms_error"),
            arg("iteration_count"),
            arg("termination_criteria"),
            arg("solution_state"),
            arg("solution_covariance"),
            arg("solution_frisbee_covariance"),
            arg("steps")
        )
        .def("__str__", &(shiftToString<LeastSquaresSolver::Analysis>))
        .def("__repr__", &(shiftToString<LeastSquaresSolver::Analysis>))
        .def_readonly(
            "rms_error",
            &LeastSquaresSolver::Analysis::rmsError,
            R"doc(
                The RMS error.

                :type: float
            )doc"
        )
        .def_readonly(
            "iteration_count",
            &LeastSquaresSolver::Analysis::iterationCount,
            R"doc(
                The iteration count.

                :type: int
            )doc"
        )
        .def_readonly(
            "termination_criteria",
            &LeastSquaresSolver::Analysis::terminationCriteria,
            R"doc(
                The termination criteria.

                :type: str
            )doc"
        )
        .def_readonly(
            "solution_state",
            &LeastSquaresSolver::Analysis::solutionState,
            R"doc(
                The solution state.

                :type: State
            )doc"
        )
        .def_readonly(
            "solution_covariance",
            &LeastSquaresSolver::Analysis::solutionCovariance,
            R"doc(
                The solution covariance matrix.

                :type: np.ndarray
            )doc"
        )
        .def_readonly(
            "solution_frisbee_covariance",
            &LeastSquaresSolver::Analysis::solutionFrisbeeCovariance,
            R"doc(
                The solution Frisbee covariance matrix.

                :type: np.ndarray
            )doc"
        )
        .def_readonly(
            "steps",
            &LeastSquaresSolver::Analysis::steps,
            R"doc(
                The steps.

                :type: list[LeastSquaresSolver.Step]
            )doc"
        )

        ;

    leastSquaresSolver
        .def(
            init<const Size&, const Real&>(),
            R"doc(
                Constructor.

                Args:
                    maximum_iteration_count (int): Maximum number of iterations.
                    rms_update_threshold (float): Minimum RMS threshold.
            )doc",
            arg("maximum_iteration_count"),
            arg("rms_update_threshold")
        )
        .def(
            "get_max_iteration_count",
            &LeastSquaresSolver::getMaxIterationCount,
            R"doc(
                Get the maximum iteration count.

                Returns:
                    int: The maximum iteration count.
            )doc"
        )
        .def(
            "get_rms_update_threshold",
            &LeastSquaresSolver::getRmsUpdateThreshold,
            R"doc(
                Get the RMS update threshold.

                Returns:
                    float: The RMS update threshold.
            )doc"
        )
        .def(
            "solve",
            &LeastSquaresSolver::solve,
            R"doc(
                Solve the non-linear least squares problem.

                Args:
                    initial_guess_state (State): Initial guess state.
                    reference_states (list[State]): List of reference states.
                    generate_states_callback (callable[list[State],[State, list[Instant]]]): Function to generate states.
                    initial_guess_sigmas (dict, optional): Dictionary of sigmas for initial guess.
                    reference_state_sigmas (dict, optional): Dictionary of sigmas for reference states.

                Returns:
                    LeastSquaresSolver::Analysis: The analysis of the solution.
            )doc",
            arg("initial_guess_state"),
            arg("reference_states"),
            arg("generate_states_callback"),
            arg("initial_guess_sigmas") = DEFAULT_INITIAL_GUESS_SIGMAS,
            arg("reference_state_sigmas") = DEFAULT_REFERENCE_STATE_SIGMAS
        )
        .def_static(
            "calculate_empirical_covariance",
            &LeastSquaresSolver::calculateEmpiricalCovariance,
            R"doc(
                Calculate the empirical covariance matrix from an array of state residuals.

                Args:
                    residuals (list[State]): A list of state residuals.

                Returns:
                    np.ndarray: The empirical covariance matrix.

                Throws:
                    ostk::core::error::runtime::Undefined: If the residual array is empty.
            )doc",
            arg("residuals")
        )
        .def_static(
            "default",
            &LeastSquaresSolver::Default,
            R"doc(
                Create a default instance of LeastSquaresSolver.

                Returns:
                    LeastSquaresSolver: A default instance of LeastSquaresSolver.
            )doc"
        )

        ;
}
