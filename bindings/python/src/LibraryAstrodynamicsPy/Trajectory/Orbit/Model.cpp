////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           LibraryAstrodynamicsPy/Trajectory/Orbit/Model.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     LibraryAstrodynamicsPy_Trajectory_Orbit_Model ( )
{

    using namespace boost::python ;

    using library::astro::trajectory::orbit::Model ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::SGP4 ;

    // scope in_Model = class_<Model, bases<library::astro::trajectory::Model>, boost::noncopyable>("Model", no_init)
    scope in_Model = class_<Model, boost::noncopyable>("OrbitModel", no_init)

        .def(self == self)
        .def(self != self)

        .def(self_ns::str(self_ns::self))

        .def("isDefined", &Model::isDefined)

        .def("isKepler", +[] (const Model& aModel) -> bool { return aModel.is<Kepler>() ; })
        .def("isSGP4", +[] (const Model& aModel) -> bool { return aModel.is<SGP4>() ; })

        .def("asKepler", +[] (const Model& aModel) -> const Kepler& { return aModel.as<Kepler>() ; }, return_value_policy<reference_existing_object>())
        .def("asSGP4", +[] (const Model& aModel) -> const SGP4& { return aModel.as<SGP4>() ; }, return_value_policy<reference_existing_object>())

        .def("getEpoch", &Model::getEpoch)
        .def("getRevolutionNumberAtEpoch", &Model::getRevolutionNumberAtEpoch)
        .def("calculateStateAt", &Model::calculateStateAt)
        .def("calculateRevolutionNumberAt", &Model::calculateRevolutionNumberAt)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////