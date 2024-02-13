/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Message/SpaceX.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Message(pybind11::module& aModule)
{
    // Create "message" python submodule
    auto message = aModule.def_submodule("message");

    // add objects to "message" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Message_SpaceX(message);
}
