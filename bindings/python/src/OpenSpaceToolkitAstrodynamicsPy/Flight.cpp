/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Maneuver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight(nanobind::module_& aModule)
{
    // Create "flight" python submodule
    auto flight = aModule.def_submodule("flight");

    // Add objects to "flight" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Maneuver(flight);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile(flight);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System(flight);
}
