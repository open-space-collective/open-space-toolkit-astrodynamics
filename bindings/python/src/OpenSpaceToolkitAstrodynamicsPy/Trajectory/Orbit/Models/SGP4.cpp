////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4/TLE.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4 (        pybind11::module& aModule                                 )
{

    using namespace pybind11 ;

    using ostk::astro::trajectory::orbit::models::SGP4 ;
    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        class_<SGP4, ostk::astro::trajectory::orbit::Model>(aModule, "SGP4")
        // scope in_SGP4 = class_<SGP4, bases<ostk::astro::trajectory::orbit::Model>>("SGP4", init<TLE>())

            .def(init<TLE>())

            .def(self == self)
            .def(self != self)

            // .def(self_ns::str(self_ns::self))
            // .def(self_ns::repr(self_ns::self))

            .def("is_defined", &SGP4::isDefined)

            .def("get_tle", &SGP4::getTle)
            .def("get_epoch", &SGP4::getEpoch)
            .def("get_revolution_number_at_epoch", &SGP4::getRevolutionNumberAtEpoch)
            .def("calculate_state_at", &SGP4::calculateStateAt)
            .def("calculate_revolution_number_at", &SGP4::calculateRevolutionNumberAt)

        ;

    }

    // Create "sgp4" python submodule
    auto sgp4 = aModule.def_submodule("sgp4") ;

    // Add __path__ attribute for "sgp4" submodule
    sgp4.attr("__path__") = "ostk.astrodynamics.sgp4" ;

    // Add objects to "sgp4" python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4_TLE(sgp4) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
