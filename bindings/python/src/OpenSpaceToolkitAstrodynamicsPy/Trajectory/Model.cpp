/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using BaseModel = ostk::astro::trajectory::Model;

    class_<BaseModel>(
        aModule,
        "Model",
        R"doc(
            Orbital model.

            Group:
                trajectory
        )doc"
    )
        .def(
            "__eq__",
            [](const BaseModel &self, const BaseModel &other)
            {
                return self == other;
            }
        )
        .def(
            "__ne__",
            [](const BaseModel &self, const BaseModel &other)
            {
                return self != other;
            }
        )

        .def("__str__", &(shiftToString<BaseModel>))
        .def("__repr__", &(shiftToString<BaseModel>))

        .def("is_defined", &BaseModel::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.

            )doc"
        )

        .def("calculate_state_at", &BaseModel::calculateStateAt, arg("instant"),
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.

            )doc"
        )
        .def("calculate_states_at", &BaseModel::calculateStatesAt, arg("instants"),
            R"doc(
                Calculate the states at given instants.

                @param instants The instants.

                Returns:
                    Array<State>: The states at the given instants.

            )doc"
        )

        ;
}