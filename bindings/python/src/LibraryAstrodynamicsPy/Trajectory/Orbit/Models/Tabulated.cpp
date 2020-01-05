////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Tabulated.cpp
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

        .def("isDefined", &Tabulated::isDefined)

        .def("getEpoch", &Tabulated::getEpoch)
        .def("getRevolutionNumberAtEpoch", &Tabulated::getRevolutionNumberAtEpoch)
        .def("calculateStateAt", &Tabulated::calculateStateAt)
        .def("calculateRevolutionNumberAt", &Tabulated::calculateRevolutionNumberAt)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
