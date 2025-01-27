/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Data/Provider.cpp>

inline void OpenSpaceAstrodynamicsPy_Data(pybind11::module& aModule)
{
    using namespace pybind11;

    // Create "data" python submodule
    auto data = aModule.def_submodule("data");

    // Add objects to python "data" submodules
    OpenSpaceToolkitAstrodynamicsPy_Data_Provider(data);
}
