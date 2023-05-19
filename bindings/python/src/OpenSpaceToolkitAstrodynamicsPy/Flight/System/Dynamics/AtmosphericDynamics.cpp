/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDynamics.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDynamics(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::AtmosphericDynamics;

    {
        class_<AtmosphericDynamics, Dynamics, Shared<AtmosphericDynamics>>(aModule, "AtmosphericDynamics")
            .def(init<const Shared<Celestial>&, SatelliteSystem&>(), arg("celestial"), arg("satellite_system"))

            .def("__str__", &(shiftToString<AtmosphericDynamics>))
            .def("__repr__", &(shiftToString<AtmosphericDynamics>))

            .def("is_defined", &AtmosphericDynamics::isDefined)

            .def("update", &AtmosphericDynamics::update, arg("x"), arg("dxdt"), arg("instant"))

            .def("get_celestial", &AtmosphericDynamics::getCelestial)
            .def("get_satellite_system", &AtmosphericDynamics::getSatelliteSystem);
    }
}
