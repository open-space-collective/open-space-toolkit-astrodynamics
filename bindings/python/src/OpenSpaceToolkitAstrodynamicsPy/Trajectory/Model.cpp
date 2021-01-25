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

    using ostk::astro::trajectory::Model ;

    class_<Model>(aModule, "Model")

        // .def(self == self)
        // .def(self != self)
        // Equivalence with default constructor deleted for class "Model"
        .def("__eq__", [](const Model &self, const Model &other){ return self == other; })
        .def("__ne__", [](const Model &self, const Model &other){ return self != other; })

        // .def(self_ns::str(self_ns::self))
        // .def(self_ns::repr(self_ns::self))

        .def("is_defined", &Model::isDefined)

        .def("calculate_state_at", &Model::calculateStateAt)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
