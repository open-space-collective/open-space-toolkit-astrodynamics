/// Apache License 2.0

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add __path__ attribute for "dynamics" submodule
    dynamics.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics";

    // Add objects to "dynamics" submodule
}
