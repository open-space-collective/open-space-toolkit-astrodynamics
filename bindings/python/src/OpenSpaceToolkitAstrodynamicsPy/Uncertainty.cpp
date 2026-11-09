/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Uncertainty/Covariance.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Uncertainty(nanobind::module_& aModule)
{
    using namespace nanobind;

    // Create "uncertainty" python submodule
    auto uncertainty = aModule.def_submodule("uncertainty");

    // Add objects to python "uncertainty" submodules
    OpenSpaceToolkitAstrodynamicsPy_Uncertainty_Covariance(uncertainty);
}
