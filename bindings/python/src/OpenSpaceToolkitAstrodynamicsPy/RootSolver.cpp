/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_RootSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;
    using ostk::core::types::Size;

    using ostk::astro::RootSolver;

    typedef std::function<double(const double&)> pythonFunctionSignature;

    class_<RootSolver::Solution>(aModule, "RootSolverSolution")
        .def_readwrite(
            "root",
            &RootSolver::Solution::root,
            R"doc(
                The root of the function.

                Returns:
                    root (float): The root of the function.
            )doc"
        )
        .def_readwrite(
            "iteration_count",
            &RootSolver::Solution::iterationCount,
            R"doc(
                The number of iterations required to find the root.

                Returns:
                    iteration_count (int): The number of iterations required to find the root.
            )doc"
        )
        .def_readwrite(
            "has_converged",
            &RootSolver::Solution::hasConverged,
            R"doc(
                Whether the root solver has converged.

                Returns:
                    has_converged (bool): Whether the root solver has converged.
            )doc"
        );

    {
        class_<RootSolver>(aModule, "RootSolver")
            .def(
                init<const Size&, const Real&>(),
                R"doc(
                    Constructor.

                    Args:
                        maximum_iteration_count (int): The maximum number of iterations allowed.
                        tolerance (float): The tolerance of the root solver.

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
                        tolerance (float): The tolerance of the root solver.
                 )doc"
            )
            .def(
                "get_maximum_iteration_count",
                &RootSolver::getMaximumIterationCount,
                R"doc(
                    Get the maximum number of iterations allowed.

                    Returns:
                        maximum_iteration_count (int): The maximum number of iterations allowed.
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
                        solution (RootSolverSolution): The solution to the root.

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
                        solution (RootSolverSolution): The solution to the root.

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
                        solution (RootSolverSolution): The solution to the root.

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
                        root_solver (RootSolver): The default root solver.

                    Group:
                        Static methods
                )doc"
            )

            ;
    }
}
