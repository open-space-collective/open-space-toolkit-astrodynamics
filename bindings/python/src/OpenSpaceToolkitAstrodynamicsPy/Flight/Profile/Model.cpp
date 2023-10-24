/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using ostk::astro::flight::profile::Model;

    class_<Model>(
        aModule,
        "Model",
        R"doc(
            A flight profile model.

            Group:
                profile
        )doc"
    )

        .def(
            "__eq__",
            [](const Model &self, const Model &other)
            {
                return self == other;
            }
        )
        .def(
            "__ne__",
            [](const Model &self, const Model &other)
            {
                return self != other;
            }
        )

        .def("__str__", &(shiftToString<Model>))
        .def("__repr__", &(shiftToString<Model>))

        .def(
            "is_defined",
            &Model::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
            )doc"
        )

        .def(
            "calculate_state_at",
            &Model::calculateStateAt,
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
            "calculate_states_at",
            &Model::calculateStatesAt,
            R"doc(
                Calculate the states of the model at specific instants.

                Args:
                    instants (list[Instant]): The instants at which to calculate the states.

                Returns:
                    list[State]: The states of the model at the specified instants.
            )doc",
            arg("instants")
        )

        .def(
            "get_axes_at",
            &Model::getAxesAt,
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
            &Model::getBodyFrame,
            R"doc(
                Get the body frame of the model with the specified name.

                Args:
                    frame_name (str): The name of the body frame.

                Returns:
                    Frame: The body frame of the model with the specified name.
            )doc",
            arg("frame_name")
        );
}
