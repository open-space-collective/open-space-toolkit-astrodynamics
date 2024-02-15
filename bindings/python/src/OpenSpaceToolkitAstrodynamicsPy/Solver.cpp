/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Solver/FiniteDifferenceSolver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Solver/TemporalConditionSolver.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solver(pybind11::module& aModule)
{
    // Create "solver" python submodule
    auto solvers = aModule.def_submodule("solver");

    // Add objects to "solver" submodule
    OpenSpaceToolkitAstrodynamicsPy_Solver_TemporalConditionSolver(solvers);
    OpenSpaceToolkitAstrodynamicsPy_Solver_FiniteDifferenceSolver(solvers);
}
