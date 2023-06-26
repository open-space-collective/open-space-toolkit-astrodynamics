/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_PropulsionSystem(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::math::geom::d3::objects::Composite;
    using ostk::math::obj::Matrix3d;

    using ostk::physics::units::Mass;

    using ostk::astro::flight::System;
    using ostk::astro::flight::system::PropulsionSystem;

    {
        class_<PropulsionSystem, System>(aModule, "PropulsionSystem")

            .def(
                init<const Mass&, const Composite&, const Matrix3d&, const Real&, const Real&>(),
                arg("mass"),
                arg("satellite_geometry"),
                arg("inertia_tensor"),
                arg("cross_sectional_surface_area"),
                arg("drag_coefficient")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<PropulsionSystem>))
            .def("__repr__", &(shiftToString<PropulsionSystem>))

            .def("is_defined", &PropulsionSystem::isDefined)

            .def("get_inertia_tensor", &PropulsionSystem::getInertiaTensor)
            .def("get_cross_sectional_surface_area", &PropulsionSystem::getCrossSectionalSurfaceArea)
            .def("get_drag_coefficient", &PropulsionSystem::getDragCoefficient)

            .def_static("undefined", &PropulsionSystem::Undefined);
    }
}
