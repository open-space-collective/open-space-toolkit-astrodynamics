/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Message/CCSDS.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_Message(pybind11::module& aModule)
{
    // Create "message" python submodule
    auto message = aModule.def_submodule("message");

    // Add objects to "message" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Message_CCSDS(message);
}
