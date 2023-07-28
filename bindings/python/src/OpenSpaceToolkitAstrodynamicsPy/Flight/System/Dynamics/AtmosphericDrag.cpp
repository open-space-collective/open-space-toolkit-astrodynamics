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
        class_<AtmosphericDrag, Dynamics, Shared<AtmosphericDrag>>(aModule, "AtmosphericDrag")
            .def(init<const Shared<Celestial>&, SatelliteSystem&>(), arg("celestial"), arg("satellite_system"))

            .def("__str__", &(shiftToString<AtmosphericDrag>))
            .def("__repr__", &(shiftToString<AtmosphericDrag>))

            .def("is_defined", &AtmosphericDrag::isDefined)

            .def("get_celestial", &AtmosphericDrag::getCelestial)
            .def("get_satellite_system", &AtmosphericDrag::getSatelliteSystem)

            .def(
                "compute_contribution",
                &AtmosphericDrag::computeContribution,
                arg("instant"),
                arg("reduced_x"),
                arg("frame")
            );
    }
}
