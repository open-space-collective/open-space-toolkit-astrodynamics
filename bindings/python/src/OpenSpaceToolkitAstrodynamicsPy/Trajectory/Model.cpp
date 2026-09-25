/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/Nadir.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/Tabulated.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model/TargetScan.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(pybind11::module &aModule)
{
    using namespace pybind11;

    using BaseModel = ostk::astrodynamics::trajectory::Model;

    class_<BaseModel> modelClass(
        aModule,
        "Model",
        R"doc(
            Trajectory model.

        )doc"
    );

    modelClass
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

    // Errors raised when a state is requested outside of the time interval over which a model is defined, exposed as
    // nested exception types deriving from RuntimeError like every other OSTk error.
    exception<BaseModel::OutOfBoundsError> &outOfBoundsError =
        register_exception<BaseModel::OutOfBoundsError>(modelClass, "OutOfBoundsError", PyExc_RuntimeError);
    exception<BaseModel::BeforeStartError> &beforeStartError =
        register_exception<BaseModel::BeforeStartError>(modelClass, "BeforeStartError", outOfBoundsError);
    exception<BaseModel::AfterEndError> &afterEndError =
        register_exception<BaseModel::AfterEndError>(modelClass, "AfterEndError", outOfBoundsError);

    outOfBoundsError.doc() = R"doc(
        Raised when a state is requested at an instant outside of the time interval over which the model is defined
        (e.g. a tabulated model). Models raise the more specific `Model.BeforeStartError` or `Model.AfterEndError`.
    )doc";
    beforeStartError.doc() = R"doc(
        Raised when a state is requested at an instant before the start of the time interval over which the model is
        defined.
    )doc";
    afterEndError.doc() = R"doc(
        Raised when a state is requested at an instant after the end of the time interval over which the model is
        defined.
    )doc";

    // Name the exception types after the class they are nested in, like the other nested types.
    for (const auto &[error, name] :
         {std::pair<handle, const char *> {outOfBoundsError, "OutOfBoundsError"},
          std::pair<handle, const char *> {beforeStartError, "BeforeStartError"},
          std::pair<handle, const char *> {afterEndError, "AfterEndError"}})
    {
        error.attr("__module__") = modelClass.attr("__module__");
        error.attr("__qualname__") = std::string("Model.") + name;
    }

    // Create "model" python submodule
    auto model = aModule.def_submodule("model");

    // Add objects to python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Nadir(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Tabulated(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_TargetScan(model);
}
