/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystem(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::math::geometry::d3::objects::Composite;
    using ostk::math::object::Matrix3d;

    using ostk::physics::units::Mass;

    using ostk::astro::flight::System;
    using ostk::astro::flight::system::PropulsionSystem;
    using ostk::astro::flight::system::SatelliteSystem;

    {
        class_<SatelliteSystem, System>(
            aModule,
            "SatelliteSystem",
            R"doc(
                A Satellite System.

                Group:
                    system
            )doc"
        )

            .def(
                init<const Mass&, const Composite&, const Matrix3d&, const Real&, const Real&, const PropulsionSystem&>(
                ),
                R"doc(
                    Constructor.

                    Args:
                        mass (Mass): The mass of the satellite system.
                        satellite_geometry (Composite): The geometry of the satellite system.
                        inertia_tensor (Matrix3d): The inertia tensor of the satellite system.
                        cross_sectional_surface_area (float): The cross-sectional surface area of the satellite system.
                        drag_coefficient (float): The drag coefficient of the satellite system.
                        propulsion_system (PropulsionSystem): The propulsion system of the satellite system.

                    Group:
                        Constructors
                )doc",
                arg("mass"),
                arg("satellite_geometry"),
                arg("inertia_tensor"),
                arg("cross_sectional_surface_area"),
                arg("drag_coefficient"),
                arg("propulsion_system") = PropulsionSystem::Undefined()
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteSystem>))
            .def("__repr__", &(shiftToString<SatelliteSystem>))

            .def(
                "is_defined",
                &SatelliteSystem::isDefined,
                R"doc(
                    Check if the satellite system is defined.

                    Returns:
                        bool: True if the satellite system is defined, False otherwise.

                )doc"
            )

            .def(
                "get_inertia_tensor",
                &SatelliteSystem::getInertiaTensor,
                R"doc(
                    Get the inertia tensor of the satellite system.

                    Returns:
                        Matrix3d: The inertia tensor of the satellite system.

                )doc"
            )

            .def(
                "get_cross_sectional_surface_area",
                &SatelliteSystem::getCrossSectionalSurfaceArea,
                R"doc(
                    Get the cross-sectional surface area of the satellite system.

                    Returns:
                        float: The cross-sectional surface area of the satellite system.

                )doc"
            )

            .def(
                "get_drag_coefficient",
                &SatelliteSystem::getDragCoefficient,
                R"doc(
                    Get the drag coefficient of the satellite system.

                    Returns:
                        float: The drag coefficient of the satellite system.

                )doc"
            )

            .def_static(
                "undefined",
                &SatelliteSystem::Undefined,
                R"doc(
                    Create an undefined satellite system.

                    Returns:
                        SatelliteSystem: The undefined satellite system.
                )doc"
            )

            .def_static(
                "default",
                &SatelliteSystem::Default,
                R"doc(
                    Create a default satellite system.

                    Returns:
                        SatelliteSystem: The default satellite system.
                )doc"
            )

            ;
    }
}
