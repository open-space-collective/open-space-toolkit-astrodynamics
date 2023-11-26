/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_State(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::math::geometry::d3::trf::rot::Quaternion;
    using ostk::math::object::Vector3d;

    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::Instant;

    using ostk::astro::flight::profile::State;

    class_<State>(
        aModule,
        "State",
        R"doc(
            Flight profile state.

            Group:
                profile
        )doc"
    )

        .def(
            init<
                const Instant&,
                const Position&,
                const Velocity&,
                const Quaternion&,
                const Vector3d&,
                const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Args:
                    instant (Instant): The instant of the state.
                    position (Position): The position of the state.
                    velocity (Velocity): The velocity of the state.
                    attitude (Quaternion): The attitude of the state.
                    angular_velocity (Vector3d): The angular velocity of the state.
                    reference_frame (Frame): The reference frame of the state.
            )doc",
            arg("instant"),
            arg("position"),
            arg("velocity"),
            arg("attitude"),
            arg("angular_velocity"),
            arg("reference_frame")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def(
            "is_defined",
            &State::isDefined,
            R"doc(
                Check if the state is defined.

                Returns:
                    bool: True if the state is defined, False otherwise.
             )doc"
        )

        .def(
            "get_instant",
            &State::getInstant,
            R"doc(
                Get the instant of the state.

                Returns:
                    Instant: The instant of the state.
             )doc"
        )

        .def(
            "get_position",
            &State::getPosition,
            R"doc(
                Get the position of the state.

                Returns:
                    Position: The position of the state.
             )doc"
        )

        .def(
            "get_velocity",
            &State::getVelocity,
            R"doc(
                Get the velocity of the state.

                Returns:
                    Velocity: The velocity of the state.
             )doc"
        )

        .def(
            "get_attitude",
            &State::getAttitude,
            R"doc(
                Get the attitude of the state.

                Returns:
                    Quaternion: The attitude of the state.
             )doc"
        )

        .def(
            "get_angular_velocity",
            &State::getAngularVelocity,
            R"doc(
                Get the angular velocity of the state.

                Returns:
                    Vector3d: The angular velocity of the state.
             )doc"
        )

        .def(
            "get_frame",
            &State::getFrame,
            R"doc(
                Get the reference frame of the state.

                Returns:
                    Frame: The reference frame of the state.
             )doc"
        )

        .def(
            "in_frame",
            &State::inFrame,
            R"doc(
                Convert the state to a different reference frame.

                Args:
                    frame (Frame): The reference frame to convert the state to.

                Returns:
                    State: The state in the new reference frame.
             )doc",
            arg("frame")
        )

        .def_static(
            "undefined",
            &State::Undefined,
            R"doc(
                Get an undefined state.

                Returns:
                    undefined_state (State): The undefined state.
             )doc"
        )

        ;
}
