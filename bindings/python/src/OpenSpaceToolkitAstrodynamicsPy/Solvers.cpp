/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Solvers/TemporalConditionSolver.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solvers(pybind11::module& aModule)
{
    // Create "solvers" python submodule
    auto solvers = aModule.def_submodule("solvers");

    // Add __path__ attribute for "solvers" submodule
    solvers.attr("__path__") = "ostk.astrodynamics.solvers";

    // Add objects to "solvers" submodule
    OpenSpaceToolkitAstrodynamicsPy_Solvers_TemporalConditionSolver(solvers);
}
