////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/SatelliteSystem.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics//Flight/SatelliteSystem.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteDynamics.cpp>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_SatelliteSystem (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::core::types::Real ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;

    using ostk::physics::units::Mass ;

    using ostk::astro::flight::SatelliteSystem ;

    {

        class_<SatelliteSystem> satellitesystem_class(aModule, "SatelliteSystem") ;

        satellitesystem_class
            .def(init<const   Mass&, const   Matrix3d&, const   Cuboid&, const   Real&, const   Real&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteSystem>))
            .def("__repr__", &(shiftToString<SatelliteSystem>))

            .def("is_defined", &SatelliteSystem::isDefined)

            .def("get_mass", &SatelliteSystem::getMass)
            .def("get_inertia_tensor", &SatelliteSystem::getInertiaTensor)
            .def("get_geometry", &SatelliteSystem::getGeometry)
            .def("get_cross_sectional_surface_area", &SatelliteSystem::getCrossSectionalSurfaceArea)
            .def("get_drag_coefficient", &SatelliteSystem::getDragCoefficient)

            .def_static("default_yam", &SatelliteSystem::DefaultYam)

        ;

        // Create "system" python submodule
        auto system = aModule.def_submodule("system") ;

        // Add __path__ attribute for "trajectory" submodule
        system.attr("__path__") = "ostk.astrodynamics.flight.system" ;

        // Add objects to python submodule
        OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteDynamics(system) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
