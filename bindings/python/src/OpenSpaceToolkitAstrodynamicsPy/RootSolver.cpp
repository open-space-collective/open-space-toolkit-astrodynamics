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
        .def_readwrite("number_of_iterations", &RootSolver::Solution::numberOfIterations)

        ;

    {
        class_<RootSolver>(aModule, "RootSolver")

            .def(init<const Size&, const Size&>(), arg("maximum_iterations_count"), arg("number_of_digits"))

            .def("__str__", &(shiftToString<RootSolver>))
            .def("__repr__", &(shiftToString<RootSolver>))

            .def("get_number_of_digits", &RootSolver::getNumberOfDigits)
            .def("get_maximum_iterations_count", &RootSolver::getMaximumIterationsCount)

            .def(
                "solve",
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

            .def_static("default", &RootSolver::Default)

            ;
    }
}
