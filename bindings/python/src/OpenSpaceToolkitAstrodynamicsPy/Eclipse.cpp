/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Eclipse/Generator.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Eclipse(pybind11::module& aModule)
{
    // Create "eclipse" python submodule
    auto eclipse = aModule.def_submodule("eclipse");

    // Add objects to "eclipse" submodule
    OpenSpaceToolkitAstrodynamicsPy_Eclipse_Generator(eclipse);
}
