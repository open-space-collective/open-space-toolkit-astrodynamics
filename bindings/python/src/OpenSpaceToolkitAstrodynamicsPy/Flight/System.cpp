/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/PropulsionSystem.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteSystem.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::math::geom::d3::objects::Composite;

    using ostk::physics::units::Mass;

    using ostk::astro::flight::System;

    {
        class_<System>(aModule, "System")

            .def(init<const Mass&, const Composite&>(), arg("mass"), arg("geometry"))

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<System>))
            .def("__repr__", &(shiftToString<System>))

            .def("is_defined", &System::isDefined)

            .def("get_mass", &System::getMass)
            .def("get_geometry", &System::getGeometry)

            .def_static("undefined", &System::Undefined)

            ;
    }

    // Create "system" python submodule
    auto system = aModule.def_submodule("system");

    // Add __path__ attribute for "system" submodule
    system.attr("__path__") = "ostk.astrodynamics.flight.system";

    // Add objects to "system" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_PropulsionSystem(system);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystem(system);
}
