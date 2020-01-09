////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler ( )
{

    using namespace boost::python ;

    using ostk::core::types::Real ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Instant ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::kepler::COE ;

    {

        scope in_Kepler = class_<Kepler, bases<ostk::astro::trajectory::orbit::Model>>("Kepler", init<const COE&, const Instant&, const Derived&, const Length&, const Real&, const Kepler::PerturbationType&>())

            .def(init<const COE&, const Instant&, const Celestial&, const Kepler::PerturbationType&>())
            .def(init<const COE&, const Instant&, const Celestial&, const Kepler::PerturbationType&, const bool>())

            .def(self == self)
            .def(self != self)

            .def(self_ns::str(self_ns::self))
            .def(self_ns::repr(self_ns::self))

            .def("is_defined", &Kepler::isDefined)

            .def("get_classical_orbital_elements", &Kepler::getClassicalOrbitalElements)
            .def("get_epoch", &Kepler::getEpoch)
            .def("get_revolution_number_at_epoch", &Kepler::getRevolutionNumberAtEpoch)
            .def("get_gravitational_parameter", &Kepler::getGravitationalParameter)
            .def("get_equatorial_radius", &Kepler::getEquatorialRadius)
            .def("get_j2", &Kepler::getJ2)
            .def("get_perturbation_type", &Kepler::getPerturbationType)
            .def("calculate_state_at", &Kepler::calculateStateAt)
            .def("calculate_revolution_number_at", &Kepler::calculateRevolutionNumberAt)

            .def("string_from_perturbation_type", &Kepler::StringFromPerturbationType).staticmethod("string_from_perturbation_type")

        ;

        enum_<Kepler::PerturbationType>("PerturbationType")

            .value("No", Kepler::PerturbationType::None)
            .value("J2", Kepler::PerturbationType::J2)

        ;

    }

    boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("ostk.astrodynamics.trajectory.orbit.models.kepler")))) ;

    boost::python::scope().attr("kepler") = module ;

    boost::python::scope scope = module ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
