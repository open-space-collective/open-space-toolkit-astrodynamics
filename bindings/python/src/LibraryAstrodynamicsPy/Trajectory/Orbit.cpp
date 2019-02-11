////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Pass.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Models.cpp>
#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Model.cpp>

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit     ( )
{

    using namespace boost::python ;

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::ctnr::Array ;

    using library::physics::env::obj::Celestial ;
    
    using library::astro::trajectory::State ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::SGP4 ;

    scope in_Orbit = class_<Orbit, bases<library::astro::Trajectory>>("Orbit", init<const library::astro::trajectory::orbit::Model&, const Shared<const Celestial>&>())

        .def(init<const Array<State>&, const Integer&, const Shared<const Celestial>&>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Orbit::isDefined)

        .def("accessModel", &Orbit::accessModel, return_value_policy<reference_existing_object>())
        .def("accessKeplerModel", +[] (const Orbit& anOrbit) -> const Kepler& { return anOrbit.accessModel().as<Kepler>() ; }, return_value_policy<reference_existing_object>()) // [TBR]
        .def("accessSGP4Model", +[] (const Orbit& anOrbit) -> const SGP4& { return anOrbit.accessModel().as<SGP4>() ; }, return_value_policy<reference_existing_object>()) // [TBR]
        
        .def("getRevolutionNumberAt", &Orbit::getRevolutionNumberAt)
        .def("getPassAt", &Orbit::getPassAt)
        .def("getPassWithRevolutionNumber", &Orbit::getPassWithRevolutionNumber)
        .def("getOrbitalFrame", &Orbit::getOrbitalFrame)
        
        .def("Undefined", &Orbit::Undefined).staticmethod("Undefined")
        .def("Circular", &Orbit::Circular).staticmethod("Circular")
        .def("Equatorial", &Orbit::Equatorial).staticmethod("Equatorial")
        .def("CircularEquatorial", &Orbit::CircularEquatorial).staticmethod("CircularEquatorial")
        .def("SunSynchronous", &Orbit::SunSynchronous).staticmethod("SunSynchronous")

    ;

    enum_<Orbit::FrameType>("FrameType")

        .value("Undefined", Orbit::FrameType::Undefined)
        .value("NED", Orbit::FrameType::NED)
        .value("LVLH", Orbit::FrameType::LVLH)
        .value("LVLHGD", Orbit::FrameType::LVLHGD)
        .value("VVLH", Orbit::FrameType::VVLH)
        .value("QSW", Orbit::FrameType::QSW)
        .value("TNW", Orbit::FrameType::TNW)
        .value("VNC", Orbit::FrameType::VNC)

    ;

    LibraryAstrodynamicsPy_Trajectory_Orbit_Model() ;
    LibraryAstrodynamicsPy_Trajectory_Orbit_Models() ;
    LibraryAstrodynamicsPy_Trajectory_Orbit_Pass() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////