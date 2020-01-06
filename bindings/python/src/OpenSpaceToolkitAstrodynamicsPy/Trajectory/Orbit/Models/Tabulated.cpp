////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Tabulated.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Tabulated ( )
{

    using namespace boost::python ;

    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;

    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::orbit::models::Tabulated ;

    scope in_Tabulated = class_<Tabulated>("Tabulated", init<Array<State>, Integer>())

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .def("is_defined", &Tabulated::isDefined)

        .def("get_epoch", &Tabulated::getEpoch)
        .def("get_revolution_number_at_epoch", &Tabulated::getRevolutionNumberAtEpoch)
        .def("calculate_state_at", &Tabulated::calculateStateAt)
        .def("calculate_revolution_number_at", &Tabulated::calculateRevolutionNumberAt)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
