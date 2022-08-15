////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Tabulated.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Tabulated (        pybind11::module& aModule                            )
{

    using namespace pybind11 ;

    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;

    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::orbit::models::Tabulated ;

    class_<Tabulated>(aModule, "Tabulated")

        .def(init<Array<State>, Integer>(), arg("states"), arg("initial_revolution_number"))

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Tabulated>))
        .def("__repr__", &(shiftToString<Tabulated>))

        .def("is_defined", &Tabulated::isDefined)

        .def("get_epoch", &Tabulated::getEpoch)
        .def("get_revolution_number_at_epoch", &Tabulated::getRevolutionNumberAtEpoch)
        .def("calculate_state_at", &Tabulated::calculateStateAt, arg("instant"))
        .def("calculate_revolution_number_at", &Tabulated::calculateRevolutionNumberAt, arg("instant"))

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
