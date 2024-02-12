/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/PropulsionSystem.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteSystem.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteSystemBuilder.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::mathematics::geometry::d3::object::Composite;

    using ostk::physics::unit::Mass;

    using ostk::astro::flight::System;

    {
        class_<System>(
            aModule,
            "System",
            R"doc(
                A flight system.

                Provides the interface for flight systems.

                .. warning:: This class is an abstract class and cannot be instantiated.

            )doc"
        )

            .def(
                init<const Mass&, const Composite&>(),
                R"doc(
                    Constructor.

                    Args:
                        mass (Mass): The mass of the system.
                        geometry (Composite): The geometry of the system.

                )doc",
                arg("mass"),
                arg("geometry")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<System>))
            .def("__repr__", &(shiftToString<System>))

            .def(
                "is_defined",
                &System::isDefined,
                R"doc(
                    Check if the system is defined.

                    Returns:
                        bool: True if the system is defined, False otherwise.

                )doc"
            )

            .def(
                "get_mass",
                &System::getMass,
                R"doc(
                    Get the mass of the system.

                    Returns:
                        Mass: The mass of the system.

                )doc"
            )

            .def(
                "get_geometry",
                &System::getGeometry,
                R"doc(
                    Get the geometry of the system.

                    Returns:
                        Composite: The geometry of the system.

                )doc"
            )

            .def_static(
                "undefined",
                &System::Undefined,
                R"doc(
                    Create an undefined system.

                    Returns:
                        System: The undefined system.
                )doc"
            )

            ;
    }

    // Create "system" python submodule
    auto system = aModule.def_submodule("system");

    // Add objects to "system" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_PropulsionSystem(system);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystem(system);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystemBuilder(system);
}
