/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Data/Provider.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Data(nanobind::module_& aModule)
{
    using namespace nanobind;

    // Create "data" python submodule
    auto data = aModule.def_submodule("data");

    // Add objects to python "data" submodules
    OpenSpaceToolkitAstrodynamicsPy_Data_Provider(data);
}
