/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Messages/SpaceX.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages(pybind11::module& aModule)
{
    // Create "messages" python submodule
    auto messages = aModule.def_submodule("messages");

    // add objects to "messages" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX(messages);
}
