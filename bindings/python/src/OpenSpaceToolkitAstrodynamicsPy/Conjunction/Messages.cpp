/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Messages/CCSDS.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages(pybind11::module& aModule)
{
    // Create "messages" python submodule
    auto messages = aModule.def_submodule("messages");

    // Add objects to "messages" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages_CCSDS(messages);
}
