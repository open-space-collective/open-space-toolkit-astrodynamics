/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/Kepler.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/Kepler/BrouwerLyddaneMean.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/MEOE.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/Propagated.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/SGP4.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/Tabulated.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::astrodynamics::trajectory::orbit::Model;
    using ostk::astrodynamics::trajectory::orbit::model::Kepler;
    using ostk::astrodynamics::trajectory::orbit::model::Propagated;
    using ostk::astrodynamics::trajectory::orbit::model::SGP4;

    class_<Model>(
        aModule,
        "OrbitModel",
        R"doc(
            Base class for orbit models.

            Provides the interface for orbit models.

        )doc"
    )

        .def(
            "__eq__",
            [](const Model& self, const Model& other)
            {
                return self == other;
            }
        )
        .def(
            "__ne__",
            [](const Model& self, const Model& other)
            {
                return self != other;
            }
        )

        .def("__str__", &(shiftToString<Model>))

        .def(
            "is_defined",
            &Model::isDefined,
            R"doc(
                Check if the orbit model is defined.

                Returns:
                    bool: True if the orbit model is defined, False otherwise.
            )doc"
        )

        .def(
            "is_kepler",
            +[](const Model& aModel) -> bool
            {
                return aModel.is<Kepler>();
            },
            R"doc(
                Check if the orbit model is a Kepler model.

                Returns:
                    bool: True if the orbit model is a Kepler model, False otherwise.

            )doc"
        )
        .def(
            "is_sgp4",
            +[](const Model& aModel) -> bool
            {
                return aModel.is<SGP4>();
            },
            R"doc(
                Check if the orbit model is an SGP4 model.

                Returns:
                    bool: True if the orbit model is an SGP4 model, False otherwise.
            )doc"
        )
        .def(
            "is_propagated",
            +[](const Model& aModel) -> bool
            {
                return aModel.is<Propagated>();
            },
            R"doc(
                Check if the orbit model is a propagated model.

                Returns:
                    bool: True if the orbit model is a propagated model, False otherwise.

            )doc"
        )

        .def(
            "as_kepler",
            +[](const Model& aModel) -> const Kepler&
            {
                return aModel.as<Kepler>();
            },
            return_value_policy::reference,
            R"doc(
                Cast the orbit model to a Kepler model.

                Returns:
                    Kepler: The Kepler model.

            )doc"
        )
        .def(
            "as_sgp4",
            +[](const Model& aModel) -> const SGP4&
            {
                return aModel.as<SGP4>();
            },
            return_value_policy::reference,
            R"doc(
                Cast the orbit model to an SGP4 model.

                Returns:
                    SGP4: The SGP4 model.

            )doc"
        )
        .def(
            "as_propagated",
            +[](const Model& aModel) -> const Propagated&
            {
                return aModel.as<Propagated>();
            },
            return_value_policy::reference,
            R"doc(
                Cast the orbit model to a propagated model.

                Returns:
                    Propagated: The propagated model.

            )doc"
        )

        .def(
            "get_epoch",
            &Model::getEpoch,
            R"doc(
                Get the epoch of the orbit model.

                Returns:
                    Instant: The epoch of the orbit model.

            )doc"
        )
        .def(
            "get_revolution_number_at_epoch",
            &Model::getRevolutionNumberAtEpoch,
            R"doc(
                Get the revolution number at the epoch of the orbit model.

                Returns:
                    int: The revolution number at the epoch of the orbit model.

            )doc"
        )
        .def(
            "calculate_state_at",
            &Model::calculateStateAt,
            R"doc(
                Calculate the state of the orbit model at a given instant.

                Args:
                    instant (Instant): The instant at which to calculate the state.

                Returns:
                    State: The state of the orbit model at the given instant.

            )doc",
            arg("instant")
        )
        .def(
            "calculate_revolution_number_at",
            &Model::calculateRevolutionNumberAt,
            R"doc(
                Calculate the revolution number of the orbit model at a given instant.

                Args:
                    instant (Instant): The instant at which to calculate the revolution number.

                Returns:
                    int: The revolution number of the orbit model at the given instant.

            )doc",
            arg("instant")
        )

        ;

    // Create "model" python submodule
    auto model = aModule.def_submodule("model");

    // add objects to "model" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Kepler(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Tabulated(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Propagated(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_BrouwerLyddaneMean(model);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_MEOE(model);
}
