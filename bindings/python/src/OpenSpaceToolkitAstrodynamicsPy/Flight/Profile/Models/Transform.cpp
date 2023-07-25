/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Transform.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Models_Transform(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::Shared;

    using ostk::physics::coord::Frame;
    using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic;

    using ostk::astro::flight::profile::Model;
    using ostk::astro::flight::profile::State;
    using ostk::astro::flight::profile::models::Transform;

    class_<Transform, Model>(aModule, "Transform")

        .def(init<const DynamicProvider&, const Shared<const Frame>&>(), arg("dynamic_provider"), arg("frame"))

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def("is_defined", &Transform::isDefined)

        .def("calculate_state_at", &Transform::calculateStateAt, arg("instant"))
        .def("get_axes_at", &Transform::getAxesAt, arg("instant"))
        .def("get_body_frame", &Transform::getBodyFrame, arg("frame_name"))

        .def_static("undefined", &Transform::Undefined)
        .def_static("inertial_pointing", &Transform::InertialPointing, arg("trajectory"), arg("quaternion"))
        .def_static("nadir_pointing", &Transform::NadirPointing, arg("orbit"), arg("orbital_frame_type"))

        ;
}
