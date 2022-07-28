////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteSystem.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteSystem (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::core::types::Real ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Composite ;

    using ostk::physics::units::Mass ;

    using ostk::astro::flight::System ;
    using ostk::astro::flight::system::SatelliteSystem ;

    {

        class_<SatelliteSystem, System> satellitesystem_class(aModule, "SatelliteSystem") ;

        satellitesystem_class
            .def(init<const   Mass&, const   Composite&, const   Matrix3d&, const   Real&, const   Real&>())
            .def(init<const   SatelliteSystem&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteSystem>))
            .def("__repr__", &(shiftToString<SatelliteSystem>))

            .def("is_defined", &SatelliteSystem::isDefined)

            .def("get_inertia_tensor", &SatelliteSystem::getInertiaTensor)
            .def("get_cross_sectional_surface_area", &SatelliteSystem::getCrossSectionalSurfaceArea)
            .def("get_drag_coefficient", &SatelliteSystem::getDragCoefficient)

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
