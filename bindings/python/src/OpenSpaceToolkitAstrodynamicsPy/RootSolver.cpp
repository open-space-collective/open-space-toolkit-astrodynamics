/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_RootSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;
    using ostk::core::types::Size;

    using ostk::astro::RootSolver;

    typedef std::function<double(const double&)> pythonFunctionSignature;

    class_<RootSolver> rootSolver(
        aModule,
        "RootSolver",
        R"doc(
                A root solver is an algorithm for finding a zero-crossing of a function.

                Group:
                    solvers
            )doc"
    );

    class_<RootSolver::Solution>(
        rootSolver,
        "Solution",
        R"doc(
            A root solver solution.

            Group:
                solvers
        )doc"
    )
        .def_readwrite(
            "root",
            &RootSolver::Solution::root,
            R"doc(
                The root of the function.

                Type:
                    float
            )doc"
        )
        .def_readwrite(
            "iteration_count",
            &RootSolver::Solution::iterationCount,
            R"doc(
                The number of iterations required to find the root.

                Type:
                    int
            )doc"
        )
        .def_readwrite(
            "has_converged",
            &RootSolver::Solution::hasConverged,
            R"doc(
                Whether the root solver has converged.

                Type:
                    bool
            )doc"
        )

        ;

    {
        rootSolver

            .def(
                init<const Size&, const Real&>(),
                R"doc(
                    Constructor.

                    Args:
                        int: The maximum number of iterations allowed.
                        float: The tolerance of the root solver.

                )doc",
                arg("maximum_iteration_count"),
                arg("tolerance")
            )

            .def("__str__", &(shiftToString<RootSolver>))
            .def("__repr__", &(shiftToString<RootSolver>))

            .def(
                "get_tolerance",
                &RootSolver::getTolerance,
                R"doc(
                    Get the tolerance of the root solver.

                    Returns:
                        float: The tolerance of the root solver.
                 )doc"
            )
            .def(
                "get_maximum_iteration_count",
                &RootSolver::getMaximumIterationCount,
                R"doc(
                    Get the maximum number of iterations allowed.

                    Returns:
                        int: The maximum number of iterations allowed.
                )doc"
            )

            .def(
                "bracket_and_solve",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& anInitialGuess,
                    const bool& isRising) -> RootSolver::Solution
                {
                    return aRootSolver.bracketAndSolve(aFunction, anInitialGuess, isRising);
                },
                R"doc(
                    Bracket and solve the root of a function.

                    Args:
                        function (callable): The function to solve.
                        initial_guess (float): The initial guess for the root.
                        is_rising (bool): Whether the function is rising.

                    Returns:
                        RootSolverSolution: The solution to the root.

                )doc",
                arg("function"),
                arg("initial_guess"),
                arg("is_rising")
            )

            .def(
                "solve",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& aLowerBound,
                    const Real& anUpperBound) -> RootSolver::Solution
                {
                    return aRootSolver.solve(aFunction, aLowerBound, anUpperBound);
                },
                R"doc(
                    Solve the root of a function.

                    Args:
                        function (callable): The function to solve.
                        lower_bound (float): The lower bound of the root.
                        upper_bound (float): The upper bound of the root.

                    Returns:
                        RootSolverSolution: The solution to the root.

                )doc",
                arg("function"),
                arg("lower_bound"),
                arg("upper_bound")
            )

            .def(
                "bisection",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& aLowerBound,
                    const Real& anUpperBound) -> RootSolver::Solution
                {
                    return aRootSolver.bisection(aFunction, aLowerBound, anUpperBound);
                },
                R"doc(
                    Solve the root of a function using the bisection method.

                    Args:
                        function (callable): The function to solve.
                        lower_bound (float): The lower bound of the root.
                        upper_bound (float): The upper bound of the root.

                    Returns:
                        RootSolverSolution: The solution to the root.

                )doc",
                arg("function"),
                arg("lower_bound"),
                arg("upper_bound")
            )

            .def_static(
                "default",
                &RootSolver::Default,
                R"doc(
                    Return the default root solver.

                    Returns:
                        RootSolver: The default root solver.
                )doc"
            )

            ;
    }
}
