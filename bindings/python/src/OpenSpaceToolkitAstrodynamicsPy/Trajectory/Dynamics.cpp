/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Dynamics/AtmosphericDrag.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Dynamics/CentralBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Dynamics/PositionDerivative.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Dynamics/ThirdBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Dynamics/Thrust.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::astro::trajectory::Dynamics;

    class_<Dynamics, Shared<Dynamics>>(aModule, "Dynamics")
        .def("get_name", &Dynamics::getName)
        .def_static("get_dynamical_equations", &Dynamics::GetDynamicalEquations, arg("dynamics_array"), arg("instant"));

    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add __path__ attribute for "dynamics" submodule
    dynamics.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics";

    // Add objects to "dynamics" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_PositionDerivative(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_CentralBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_ThirdBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDrag(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thrust(dynamics);
}
