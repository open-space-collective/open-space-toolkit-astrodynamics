// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using ostk::astro::flight::profile::Model;

    class_<Model>(aModule, "Model")

        .def("__eq__",
             [](const Model &self, const Model &other) {
                 return self == other;
             })
        .def("__ne__",
             [](const Model &self, const Model &other) {
                 return self != other;
             })

        .def("__str__", &(shiftToString<Model>))
        .def("__repr__", &(shiftToString<Model>))

        .def("is_defined", &Model::isDefined)

        .def("calculate_state_at", &Model::calculateStateAt, arg("instant"))
        .def("calculate_states_at", &Model::calculateStatesAt, arg("instants"))
        .def("get_axes_at", &Model::getAxesAt, arg("instant"))
        .def("get_body_frame", &Model::getBodyFrame, arg("frame_name"))

        ;
}
