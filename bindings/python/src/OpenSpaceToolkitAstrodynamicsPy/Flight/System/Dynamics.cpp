/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/AtmosphericDynamics.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/GravitationalDynamics.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::astro::flight::system::Dynamics;

    class_<Dynamics, Shared<Dynamics>>(aModule, "Dynamics")
        .def_static("get_dynamical_equations", &Dynamics::GetDynamicalEquations, arg("instant"), arg("dynamics_array"))

        ;

    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add __path__ attribute for "dynamics" submodule
    dynamics.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics";

    // Add objects to "dynamics" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_GravitationalDynamics(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDynamics(dynamics);
}
