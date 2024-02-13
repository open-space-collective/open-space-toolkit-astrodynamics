/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Message.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction(pybind11::module& aModule)
{
    // Create "conjunction" python submodule
    auto conjunction = aModule.def_submodule("conjunction");

    // Add objects to "conjunction" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Message(conjunction);
}
