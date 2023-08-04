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

        .def_readwrite("root", &RootSolver::Solution::root)
        .def_readwrite("iteration_count", &RootSolver::Solution::iterationCount)
        .def_readwrite("has_converged", &RootSolver::Solution::hasConverged)

        ;

    {
        class_<RootSolver>(aModule, "RootSolver")

            .def(init<const Size&, const Real&>(), arg("maximum_iteration_count"), arg("tolerance"))

            .def("__str__", &(shiftToString<RootSolver>))
            .def("__repr__", &(shiftToString<RootSolver>))

            .def("get_tolerance", &RootSolver::getTolerance)
            .def("get_maximum_iteration_count", &RootSolver::getMaximumIterationCount)

            .def(
                "bracket_and_solve",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& anInitialGuess,
                    const bool& isRising)
                {
                    return aRootSolver.solve(aFunction, anInitialGuess, isRising);
                },
                arg("function"),
                arg("initial_guess"),
                arg("is_rising")
            )

            .def(
                "solve",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& aLowerBound,
                    const Real& anUpperBound)
                {
                    return aRootSolver.solve(aFunction, aLowerBound, anUpperBound);
                },
                arg("function"),
                arg("lower_bound"),
                arg("upper_bound")
            )

            .def(
                "bisection",
                +[](const RootSolver& aRootSolver,
                    const pythonFunctionSignature& aFunction,
                    const Real& aLowerBound,
                    const Real& anUpperBound)
                {
                    return aRootSolver.bisection(aFunction, aLowerBound, anUpperBound);
                },
                arg("function"),
                arg("lower_bound"),
                arg("upper_bound")
            )

            .def_static("default", &RootSolver::Default)

            ;
    }
}
