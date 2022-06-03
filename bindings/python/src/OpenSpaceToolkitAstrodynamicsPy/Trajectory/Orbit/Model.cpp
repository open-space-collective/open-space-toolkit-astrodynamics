////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model (        pybind11::module&   aModule                                     )
{

    using namespace pybind11 ;

    // using BaseModel = ostk::astro::trajectory::Model ;

    using ostk::astro::trajectory::orbit::Model ;
    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::SGP4 ;
    using ostk::astro::trajectory::orbit::models::Propagated ;

    // scope in_Model = class_<Model, bases<ostk::astro::trajectory::Model>, boost::noncopyable>("OrbitModel", no_init)
    // scope in_Model = class_<Model, bases<ostk::astro::trajectory::Model>>("OrbitModel", no_init)
    class_<Model>(aModule, "OrbitModel")

        // no init

        // .def(self == self)
        // .def(self != self)
        .def("__eq__", [](const Model &self, const Model &other){ return self == other; })
        .def("__ne__", [](const Model &self, const Model &other){ return self != other; })

        .def("__str__", &(shiftToString<Model>))

        .def("is_defined", &Model::isDefined)

        .def("is_kepler", +[] (const Model& aModel) -> bool { return aModel.is<Kepler>() ; })
        .def("is_sgp4", +[] (const Model& aModel) -> bool { return aModel.is<SGP4>() ; })
        .def("is_propagated", +[] (const Model& aModel) -> bool { return aModel.is<Propagated>() ; })

        // .def("as_kepler", +[] (const Model& aModel) -> const Kepler& { return aModel.as<Kepler>() ; }, return_value_policy<reference_existing_object>())
        // .def("as_sgp4", +[] (const Model& aModel) -> const SGP4& { return aModel.as<SGP4>() ; }, return_value_policy<reference_existing_object>())
        // .def("as_propagated", +[] (const Model& aModel) -> const Propagated& { return aModel.as<Propagated>() ; }, return_value_policy<reference_existing_object>())

        .def("as_kepler", +[] (const Model& aModel) -> const Kepler& { return aModel.as<Kepler>() ; }, return_value_policy::reference)
        .def("as_sgp4", +[] (const Model& aModel) -> const SGP4& { return aModel.as<SGP4>() ; }, return_value_policy::reference)
        .def("as_propagated", +[] (const Model& aModel) -> const Propagated& { return aModel.as<Propagated>() ; }, return_value_policy::reference)

        .def("get_epoch", &Model::getEpoch)
        .def("get_revolution_number_at_epoch", &Model::getRevolutionNumberAtEpoch)
        .def("calculate_state_at", &Model::calculateStateAt)
        .def("calculate_revolution_number_at", &Model::calculateRevolutionNumberAt)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
