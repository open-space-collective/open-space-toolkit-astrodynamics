/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Transform.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model_Transform(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;

    using ostk::astrodynamics::flight::profile::Model;
    using ostk::astrodynamics::flight::profile::model::Transform;
    using ostk::astrodynamics::trajectory::State;

    class_<Transform, Model>(
        aModule,
        "Transform",
        R"doc(
            A flight profile model defined by a transform.

        )doc"
    )

        .def(
            init<const DynamicProvider&, const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Args:
                    dynamic_provider (DynamicProvider): The dynamic provider of the transform.
                    frame (Frame): The frame of the transform.

             )doc",
            arg("dynamic_provider"),
            arg("frame")
        )

        .def("__str__", &(shiftToString<Transform>))
        .def("__repr__", &(shiftToString<Transform>))

        .def(
            "is_defined",
            &Transform::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
             )doc"
        )

        .def(
            "calculate_state_at",
            &Transform::calculateStateAt,
            R"doc(
                Calculate the state of the model at a specific instant.

                Args:
                    instant (Instant): The instant at which to calculate the state.

                Returns:
                    State: The state of the model at the specified instant.
             )doc",
            arg("instant")
        )

        .def(
            "get_axes_at",
            &Transform::getAxesAt,
            R"doc(
                Get the axes of the model at a specific instant.

                Args:
                    instant (Instant): The instant at which to get the axes.

                Returns:
                    numpy.ndarray: The axes of the model at the specified instant.
             )doc",
            arg("instant")
        )

        .def(
            "get_body_frame",
            &Transform::getBodyFrame,
            R"doc(
                Get the body frame of the model with the specified name.

                Args:
                    frame_name (str): The name of the body frame.

                Returns:
                    Frame: The body frame of the model with the specified name.
             )doc",
            arg("frame_name")
        )

        .def_static(
            "undefined",
            &Transform::Undefined,
            R"doc(
                Get an undefined transform.

                Returns:
                    Transform: The undefined transform.
             )doc"
        )

        .def_static(
            "inertial_pointing",
            &Transform::InertialPointing,
            R"doc(
                Create a transform for inertial pointing.

                Args:
                    trajectory (Trajectory): The trajectory to point at.
                    quaternion (Quaternion): The quaternion to rotate the axes by.

                Returns:
                    Transform: The transform for inertial pointing.
             )doc",
            arg("trajectory"),
            arg("quaternion")
        )

        .def_static(
            "nadir_pointing",
            &Transform::NadirPointing,
            R"doc(
                Create a transform for nadir pointing.

                Args:
                    orbit (Orbit): The orbit to point at.
                    orbital_frame_type (OrbitalFrameType): The type of the orbital frame.

                Returns:
                    Transform: The transform for nadir pointing.
             )doc",
            arg("orbit"),
            arg("orbital_frame_type")
        )

        ;
}
