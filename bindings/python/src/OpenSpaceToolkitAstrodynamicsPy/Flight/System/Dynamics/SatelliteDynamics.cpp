// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_SatelliteDynamics(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::Environment;
    using ostk::physics::time::Instant;

    using ostk::astro::flight::system::SatelliteSystem;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;

    {
        class_<SatelliteDynamics>(aModule, "SatelliteDynamics")

            .def(init<const Environment&, const SatelliteSystem&>(), arg("environment"), arg("satellite_system"))

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteDynamics>))
            .def("__repr__", &(shiftToString<SatelliteDynamics>))

            .def("is_defined", &SatelliteDynamics::isDefined)

            .def("get_instant", &SatelliteDynamics::getInstant)
            .def("set_instant", &SatelliteDynamics::setInstant, arg("instant"))
            .def("get_dynamical_equations", &SatelliteDynamics::getDynamicalEquations)

            ;
    }
}
