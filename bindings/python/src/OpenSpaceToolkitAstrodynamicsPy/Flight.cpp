/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight(pybind11::module& aModule)
{
    // Create "flight" python submodule
    auto flight = aModule.def_submodule("flight");

    // Add __path__ attribute for "flight" submodule
    flight.attr("__path__") = "ostk.astrodynamics.flight";

    // Add objects to "flight" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(flight);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System(flight);
}
