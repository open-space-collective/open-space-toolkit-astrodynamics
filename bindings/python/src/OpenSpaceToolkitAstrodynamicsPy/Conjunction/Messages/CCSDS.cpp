// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Messages/CCSDS/CDM.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages_CCSDS(pybind11::module& aModule)
{
    // Create "ccsds" python submodule
    auto ccsds = aModule.def_submodule("ccsds");

    // Add __path__ attribute for "ccsds" submodule
    ccsds.attr("__path__") = "ostk.astrodynamics.conjunction.messages.ccsds";

    // Add objects to "ccsds" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages_CCSDS_CDM(ccsds);
}
