/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/SatelliteDynamics.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/FullSatelliteDynamics.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add __path__ attribute for "dynamics" submodule
    dynamics.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics";

    // Add objects to "dynamics" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_SatelliteDynamics(dynamics) ;
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_FullSatelliteDynamics(dynamics) ;

}
