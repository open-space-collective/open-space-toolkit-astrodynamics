////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Models/Kepler.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LibraryAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp>

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Models_Kepler ( )
{

    using namespace boost::python ;

    using library::core::types::Real ;
    
    using library::physics::units::Length ;
    using library::physics::units::Derived ;
    using library::physics::time::Instant ;
    using library::physics::env::obj::Celestial ;

    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    scope in_Kepler = class_<Kepler, bases<library::astro::trajectory::orbit::Model>>("Kepler", init<const COE&, const Instant&, const Derived&, const Length&, const Real&, const Kepler::PerturbationType&>())

        .def(init<const COE&, const Instant&, const Celestial&, const Kepler::PerturbationType&>())
        .def(init<const COE&, const Instant&, const Celestial&, const Kepler::PerturbationType&, const bool>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("isDefined", &Kepler::isDefined)

        .def("getClassicalOrbitalElements", &Kepler::getClassicalOrbitalElements)
        .def("getEpoch", &Kepler::getEpoch)
        .def("getRevolutionNumberAtEpoch", &Kepler::getRevolutionNumberAtEpoch)
        .def("getGravitationalParameter", &Kepler::getGravitationalParameter)
        .def("getEquatorialRadius", &Kepler::getEquatorialRadius)
        .def("getJ2", &Kepler::getJ2)
        .def("getPerturbationType", &Kepler::getPerturbationType)
        .def("calculateStateAt", &Kepler::calculateStateAt)
        .def("calculateRevolutionNumberAt", &Kepler::calculateRevolutionNumberAt)
        
        .def("StringFromPerturbationType", &Kepler::StringFromPerturbationType).staticmethod("StringFromPerturbationType")

    ;

    enum_<Kepler::PerturbationType>("PerturbationType")

        .value("No", Kepler::PerturbationType::None)
        .value("J2", Kepler::PerturbationType::J2)

    ;

    LibraryAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////