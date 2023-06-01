/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/Messages.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction(pybind11::module& aModule)
{
    // Create "conjunction" python submodule
    auto conjunction = aModule.def_submodule("conjunction");

    // Add __path__ attribute for "conjunction" submodule
    conjunction.attr("__path__") = "ostk.astrodynamics.conjunction";

    // Add objects to "conjunction" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages(conjunction);
}
