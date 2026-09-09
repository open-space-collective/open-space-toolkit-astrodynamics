/// Apache License 2.0

#include <nanobind/nanobind.h>

#include <OpenSpaceToolkitAstrodynamicsPy/Data/Provider/OffNadir.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Data_Provider(nanobind::module_& aModule)
{
    using namespace nanobind;

    // Create "provider" python submodule
    auto provider = aModule.def_submodule("provider");

    // Add objects to python "provider" submodules
    OpenSpaceToolkitAstrodynamicsPy_Data_Provider_OffNadir(provider);
}
