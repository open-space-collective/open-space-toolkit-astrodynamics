/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDrag(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::AtmosphericDrag;

    {
<<<<<<< HEAD
        class_<AtmosphericDrag, Shared<AtmosphericDrag>, Dynamics>(aModule, "AtmosphericDrag")
=======
        class_<AtmosphericDrag, Dynamics, Shared<AtmosphericDrag>>(aModule, "AtmosphericDrag")
>>>>>>> feat: add missing files
            .def(init<const Shared<Celestial>&, SatelliteSystem&>(), arg("celestial"), arg("satellite_system"))

            .def("__str__", &(shiftToString<AtmosphericDrag>))
            .def("__repr__", &(shiftToString<AtmosphericDrag>))

            .def("is_defined", &AtmosphericDrag::isDefined)

<<<<<<< HEAD
            .def("get_celestial", &AtmosphericDrag::getCelestial)
            .def("get_satellite_system", &AtmosphericDrag::getSatelliteSystem)

            .def("update", &AtmosphericDrag::update, arg("x"), arg("dxdt"), arg("instant"));
=======
            .def("update", &AtmosphericDrag::update, arg("x"), arg("dxdt"), arg("instant"))

            .def("get_celestial", &AtmosphericDrag::getCelestial)
            .def("get_satellite_system", &AtmosphericDrag::getSatelliteSystem);
>>>>>>> feat: add missing files
    }
}
