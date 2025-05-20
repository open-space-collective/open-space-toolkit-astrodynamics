/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/Nadir.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/Tabulated.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/TargetScan.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using BaseModel = ostk::astrodynamics::trajectory::Model;

    class_<BaseModel>(
        aModule,
        "Model",
        R"doc(
            Trajectory model.

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

        .def(
            "is_defined",
            &BaseModel::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.

            )doc"
        )

        .def(
            "calculate_state_at",
            &BaseModel::calculateStateAt,
            arg("instant"),
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.

            )doc"
        )
        .def(
            "calculate_states_at",
            &BaseModel::calculateStatesAt,
            arg("instants"),
            R"doc(
                Calculate the states at given instants. It can be more performant than looping `calculate_state_at` for multiple instants.

                @param instants The instants.

                Returns:
                    Array<State>: The states at the given instants.

            )doc"
        )

        ;

    // Create "model" python submodule
    auto model = aModule.def_submodule("model");

    // Add objects to python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Nadir(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Tabulated(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_TargetScan(model);
}
