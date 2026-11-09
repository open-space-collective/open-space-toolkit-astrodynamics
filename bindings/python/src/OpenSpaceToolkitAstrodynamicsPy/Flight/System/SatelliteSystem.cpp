/// Apache License 2.0

#include <optional>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystem(nanobind::module_& aModule)
{
    using namespace nanobind;

    using ostk::mathematics::geometry::d3::object::Composite;
    using ostk::mathematics::object::Matrix3d;

    using ostk::physics::unit::Mass;

    using ostk::astrodynamics::flight::System;
    using ostk::astrodynamics::flight::system::PropulsionSystem;
    using ostk::astrodynamics::flight::system::SatelliteSystem;

    {
        class_<SatelliteSystem, System>(
            aModule,
            "SatelliteSystem",
            R"doc(
                A Satellite System.

            )doc"
        )

            .def(
                init<
                    const Mass&,
                    const Composite&,
                    const Matrix3d&,
                    const std::optional<double>&,
                    const std::optional<double>&,
                    const std::optional<PropulsionSystem>&>(),
                R"doc(
                    Constructor.

                    Args:
                        mass (Mass): The mass of the satellite system.
                        satellite_geometry (Composite): The geometry of the satellite system.
                        inertia_tensor (np.ndarray): The inertia tensor of the satellite system.
                        cross_sectional_surface_area (float | None): The cross-sectional surface area of the satellite system, or None if absent.
                        drag_coefficient (float | None): The drag coefficient of the satellite system, or None if absent.
                        propulsion_system (PropulsionSystem | None): The propulsion system of the satellite system, or None if absent.

                )doc",
                arg("mass"),
                arg("satellite_geometry"),
                arg("inertia_tensor"),
                arg("cross_sectional_surface_area"),
                arg("drag_coefficient"),
                arg("propulsion_system") = none()
            )

            .def(
                "__eq__",
                [](const SatelliteSystem& self, const SatelliteSystem& other)
                {
                    return self == other;
                },
                is_operator()
            )
            .def(
                "__ne__",
                [](const SatelliteSystem& self, const SatelliteSystem& other)
                {
                    return self != other;
                },
                is_operator()
            )

            .def("__str__", &(shiftToString<SatelliteSystem>))
            .def("__repr__", &(shiftToString<SatelliteSystem>))

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

                    Raises:
                        RuntimeError: If the satellite system has no cross-sectional surface area.

                )doc"
            )

            .def(
                "get_drag_coefficient",
                &SatelliteSystem::getDragCoefficient,
                R"doc(
                    Get the drag coefficient of the satellite system.

                    Returns:
                        float: The drag coefficient of the satellite system.

                    Raises:
                        RuntimeError: If the satellite system has no drag coefficient.

                )doc"
            )

            .def(
                "get_propulsion_system",
                &SatelliteSystem::getPropulsionSystem,
                R"doc(
                    Get the propulsion system of the satellite system.

                    Returns:
                        PropulsionSystem: The propulsion system of the satellite system.

                    Raises:
                        RuntimeError: If the satellite system has no propulsion system.

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
