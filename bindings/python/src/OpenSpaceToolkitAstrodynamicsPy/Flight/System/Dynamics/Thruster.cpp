/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/Thruster/ConstantThrustThruster.cpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::Thruster;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

// // Trampoline class for virtual member functions
// class PyThruster : public Thruster
// {
//    public:
//     using Thruster::Thruster;

//     // Trampoline (need one for each virtual function)

//     void print(std::ostream& anOutputStream, bool displayDecorator) const override
//     {
//         PYBIND11_OVERRIDE(void, Thruster, print, anOutputStream, displayDecorator);
//     }

//     bool isDefined() const override
//     {
//         PYBIND11_OVERRIDE_PURE_NAME(bool, Thruster, "is_defined", isDefined);
//     }
// };

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thruster(pybind11::module& aModule)
{
    class_<Thruster, Dynamics, Shared<Thruster>>(aModule, "Thruster");

        // .def(init<const String&>(), arg("name"))

        // .def("get_name", &Thruster::getName)
        // .def("get_satelltite_system", &Thruster::getSatelliteSystem)

        // .def("__str__", &(shiftToString<Thruster>))
        // .def("__repr__", &(shiftToString<Thruster>))

        // .def("is_defined", &Thruster::isDefined)
        // .def("compute_contribution", &Thruster::computeContribution, arg("instant"), arg("state_vector"), arg("frame"))

        // ;

    // Create "thruster" python submodule
    auto thruster = aModule.def_submodule("thruster");

    // Add __path__ attribute for "thruster" submodule
    thruster.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics.thruster";

    // Add objects to "thruster" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thruster_ConstantThrustThruster(thruster);
}
