// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Messages/SpaceX.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages(pybind11::module& aModule)
{
    // Create "messages" python submodule
    auto messages = aModule.def_submodule("messages");

    // Add __path__ attribute for "messages" submodule
    messages.attr("__path__") = "ostk.astrodynamics.trajectory.orbit.messages";

    // add objects to "messages" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX(messages);
}
