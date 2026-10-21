/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Static(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::coordinate::Position;

    using ostk::astrodynamics::trajectory::model::Static;

    class_<Static, ostk::astrodynamics::trajectory::Model>(
        aModule,
        "Static",
        R"doc(
            Static trajectory model.

            This model represents a fixed position in a non quasi-inertial frame (e.g. a point on the surface of the Earth).
        )doc"
    )

        .def(
            init<const Position&, const Shared<const Frame>&>(),
            R"doc(
                Construct a `Static` object from a position.

                The computed states are expressed in the provided output frame if any, or in the frame of the provided position otherwise.

                Args:
                    position (Position): The position. Must be in a non quasi-inertial frame (e.g. ITRF).
                    output_frame (Frame, optional): The reference frame in which the computed states are expressed. The fixed position (with zero velocity in its own frame) is converted to this frame at each requested instant. Defaults to None, in which case the frame of the position is used.

                Returns:
                    Static: The `Static` object.
            )doc",
            arg("position"),
            arg("output_frame") = none()
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Static>))
        .def("__repr__", &(shiftToString<Static>))

        .def(
            "is_defined",
            &Static::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
            )doc"
        )
        .def(
            "get_frame",
            &Static::getFrame,
            R"doc(
                Get the reference frame in which the computed states are expressed.

                Returns:
                    Frame: The output reference frame.
            )doc"
        )
        .def(
            "calculate_state_at",
            &Static::calculateStateAt,
            R"doc(
                Calculate the state at a given instant, expressed in the output frame of the model.

                The output frame is the frame of the position unless an explicit output frame was provided at construction.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.
            )doc",
            arg("instant")
        )

        ;
}
