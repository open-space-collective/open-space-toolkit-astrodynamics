/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/GravitationalDynamics.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_GravitationalDynamics(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::GravitationalDynamics;

    {
        class_<GravitationalDynamics, Dynamics, Shared<GravitationalDynamics>>(aModule, "GravitationalDynamics")
            .def(init<const Shared<Celestial>&>(), arg("celestial"))

            .def("__str__", &(shiftToString<GravitationalDynamics>))
            .def("__repr__", &(shiftToString<GravitationalDynamics>))

            .def("is_defined", &GravitationalDynamics::isDefined)

            .def("update", &GravitationalDynamics::update, arg("x"), arg("dxdt"), arg("instant"))

            .def("get_celestial", &GravitationalDynamics::getCelestial)

            ;
    }
}
