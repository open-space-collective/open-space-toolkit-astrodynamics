/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystemBuilder(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::math::geometry::d3::objects::Composite;
    using ostk::math::object::Matrix3d;

    using ostk::physics::units::Mass;

    using ostk::astro::flight::System;
    using ostk::astro::flight::system::PropulsionSystem;
    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::SatelliteSystemBuilder;

    {
        class_<SatelliteSystemBuilder>(
            aModule,
            "SatelliteSystemBuilder",
            R"doc(
                A Satellite System Builder.

                Group:
                    system
            )doc"
        )

            .def(
                init<>(),
                R"doc(
                    Constructor.

                    Group:
                        Constructors
                )doc"
            )

            .def("__str__", &(shiftToString<SatelliteSystemBuilder>))
            .def("__repr__", &(shiftToString<SatelliteSystemBuilder>))

            .def(
                "with_dry_mass",
                &SatelliteSystemBuilder::withDryMass,
                R"doc(
                    Set the dry mass.

                    Args:
                        dry_mass (Mass): The dry mass.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("dry_mass")
            )

            .def(
                "with_geometry",
                &SatelliteSystemBuilder::withGeometry,
                R"doc(
                    Set the geometry.

                    Args:
                        geometry (Composite): The geometry.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("geometry")
            )

            .def(
                "with_inertia_tensor",
                &SatelliteSystemBuilder::withInertiaTensor,
                R"doc(
                    Set the inertia tensor.

                    Args:
                        inertia_tensor (Matrix3d): The inertia tensor.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("inertia_tensor")
            )

            .def(
                "with_cross_sectional_surface_area",
                &SatelliteSystemBuilder::withCrossSectionalSurfaceArea,
                R"doc(
                    Set the dry mass.

                    Args:
                        cross_sectional_surface_area (float): The cross-sectional surface area.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("cross_sectional_surface_area")
            )

            .def(
                "with_drag_coefficient",
                &SatelliteSystemBuilder::withDragCoefficient,
                R"doc(
                    Set the drag coefficient.

                    Args:
                        drag_coefficient (float): The drag coefficient.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("drag_coefficient")
            )

            .def(
                "with_propulsion_system",
                &SatelliteSystemBuilder::withPropulsionSystem,
                R"doc(
                    Set the propulsion system.

                    Args:
                        propulsion_system (PropulsionSystem): The propulsion system.

                    Returns:
                        SatelliteSystemBuilder: The builder.

                )doc",
                arg("propulsion_system")
            )

            .def(
                "build",
                &SatelliteSystemBuilder::build,
                R"doc(
                    Build a new satellite system.

                    Returns:
                        SatelliteSystem: A new satellite system.

                )doc"
            )

            .def_static(
                "default",
                &SatelliteSystemBuilder::Default,
                R"doc(
                    Create a satellite system builder with default values.

                    Returns:
                        SatelliteSystem: The satellite system builder with default values.
                )doc"
            )

            ;
    }
}
