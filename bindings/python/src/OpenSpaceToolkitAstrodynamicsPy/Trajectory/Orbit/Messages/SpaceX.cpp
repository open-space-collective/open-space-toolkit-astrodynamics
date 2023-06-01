/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Messages/SpaceX/OPM.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX(pybind11::module& aModule)
{
    // Create "spacex" python submodule
    auto spacex = aModule.def_submodule("spacex");

    // Add __path__ attribute for "spacex" submodule
    spacex.attr("__path__") = "ostk.astrodynamics.trajectory.orbit.messages.spacex";

    // add objects to "spacex" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX_OPM(spacex);
}
