/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Message/CCSDS/CDM.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_Message_CCSDS(pybind11::module& aModule)
{
    // Create "ccsds" python submodule
    auto ccsds = aModule.def_submodule("ccsds");

    // Add objects to "ccsds" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Message_CCSDS_CDM(ccsds);
}
