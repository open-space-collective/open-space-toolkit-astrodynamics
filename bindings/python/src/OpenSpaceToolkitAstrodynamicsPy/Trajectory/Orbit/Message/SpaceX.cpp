/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Message/SpaceX/OPM.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Message_SpaceX(pybind11::module& aModule)
{
    // Create "spacex" python submodule
    auto spacex = aModule.def_submodule("spacex");

    // add objects to "spacex" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Message_SpaceX_OPM(spacex);
}
