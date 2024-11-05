/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;

    using ostk::astrodynamics::flight::profile::Model;
    using ostk::astrodynamics::flight::profile::model::Tabulated;
    using ostk::astrodynamics::trajectory::State;

    class_<Tabulated, Model>(
        aModule,
        "Tabulated",
        R"doc(
            A flight profile model defined by a set of states.

        )doc"
    )

        .def(
            init<Array<State>&>(),
            R"doc(
                Constructor.

                Args:
                    states (Array[State]): The states of the model.
             )doc",
            arg("states")
        )

        .def(
            "__str__",
            &(shiftToString<Tabulated>),
            R"doc(
                Convert the model to a string.

                Returns:
                    str: The string representation of the model.
             )doc"
        )

        .def(
            "__repr__",
            &(shiftToString<Tabulated>),
            R"doc(
                Convert the model to a string.

                Returns:
                    str: The string representation of the model.
             )doc"
        )

        .def(
            "is_defined",
            &Tabulated::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
             )doc"
        )

        .def(
            "get_interval",
            &Tabulated::getInterval,
            R"doc(
                Get the interval of the model.

                Returns:
                    Interval: The interval of the model.
             )doc"
        )

        .def(
            "calculate_state_at",
            &Tabulated::calculateStateAt,
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
            &Tabulated::getAxesAt,
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
            &Tabulated::getBodyFrame,
            R"doc(
                Get the body frame of the model with the specified name.

                Args:
                    frame_name (str): The name of the body frame.

                Returns:
                    Frame: The body frame of the model with the specified name.
             )doc",
            arg("frame_name")
        )

        // .def_static("load", &Tabulated::Load, arg("file"))

        ;
}
