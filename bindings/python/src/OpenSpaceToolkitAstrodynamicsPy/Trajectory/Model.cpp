////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model (        pybind11::module&         aModule                                     )
{

    using namespace pybind11 ;

    using BaseModel = ostk::astro::trajectory::Model ;

    class_<BaseModel>(aModule, "Model")

        // .def(self == self)
        // .def(self != self)
        // Equivalence with default constructor deleted for class "Model"
        .def("__eq__", [](const BaseModel &self, const BaseModel &other){ return self == other; })
        .def("__ne__", [](const BaseModel &self, const BaseModel &other){ return self != other; })

        .def("__str__", &(shiftToString<BaseModel>))
        .def("__repr__", &(shiftToString<BaseModel>))

        .def("is_defined", &BaseModel::isDefined)

        .def("calculate_state_at", &BaseModel::calculateStateAt, arg("instant"))
        .def("calculate_states_at", &BaseModel::calculateStatesAt, arg("instants"))

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
