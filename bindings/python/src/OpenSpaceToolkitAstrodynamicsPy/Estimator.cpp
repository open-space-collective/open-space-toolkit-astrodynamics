/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Estimator/ODLeastSquaresSolver.cpp>

inline void OpenSpaceAstrodynamicsPy_Estimator(pybind11::module& aModule)
{
    using namespace pybind11;

    // Create "estimator" python submodule
    auto estimator = aModule.def_submodule("estimator");

    // Add objects to python "estimator" submodules
    OpenSpaceToolkitAstrodynamicsPy_Estimator_ODLeastSquaresSolver(estimator);
}
