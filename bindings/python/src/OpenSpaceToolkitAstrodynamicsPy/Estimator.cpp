/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Estimator/OrbitDeterminationSolver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Estimator/TLESolver.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator(nanobind::module_& aModule)
{
    using namespace nanobind;

    // Create "estimator" python submodule
    auto estimator = aModule.def_submodule("estimator");

    // Add objects to python "estimator" submodules
    OpenSpaceToolkitAstrodynamicsPy_Estimator_OrbitDeterminationSolver(estimator);
    OpenSpaceToolkitAstrodynamicsPy_Estimator_TLESolver(estimator);
}
