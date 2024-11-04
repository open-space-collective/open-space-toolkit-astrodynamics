/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model/Tabulated.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/Profile/Model/Transform.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using ostk::astrodynamics::flight::profile::Model;

    class_<Model>(
        aModule,
        "Model",
        R"doc(
            A flight profile model.

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
            "is_tabulated",
            +[](const Model &aModel) -> bool
            {
                return aModel.is<ostk::astrodynamics::flight::profile::model::Tabulated>();
            },
            R"doc(
                Check if the model is a tabulated model.

                Returns:
                    bool: True if the model is a tabulated model, False otherwise.
            )doc"
        )
        .def(
            "is_transform",
            +[](const Model &aModel) -> bool
            {
                return aModel.is<ostk::astrodynamics::flight::profile::model::Transform>();
            },
            R"doc(
                Check if the model is a transform model.

                Returns:
                    bool: True if the model is a transform model, False otherwise.
            )doc"
        )

        .def(
            "as_tabulated",
            +[](const Model &aModel) -> const ostk::astrodynamics::flight::profile::model::Tabulated &
            {
                return aModel.as<ostk::astrodynamics::flight::profile::model::Tabulated>();
            },
            R"doc(
                Cast the model to a tabulated model.

                Returns:
                    Tabulated: The tabulated model.
            )doc"
        )
        .def(
            "as_transform",
            +[](const Model &aModel) -> const ostk::astrodynamics::flight::profile::model::Transform &
            {
                return aModel.as<ostk::astrodynamics::flight::profile::model::Transform>();
            },
            R"doc(
                Cast the model to a transform model.

                Returns:
                    Transform: The transform model.
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
                Calculate the states of the model at specific instants. It can be more performant than looping `calculate_state_at` for multiple instants.

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

    // Create "model" python submodule
    auto model = aModule.def_submodule("model");

    // add objects to "model" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model_Transform(model);
    OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model_Tabulated(model);
}
