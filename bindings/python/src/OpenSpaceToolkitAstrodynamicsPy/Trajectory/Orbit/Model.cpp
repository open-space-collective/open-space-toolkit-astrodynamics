/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::astro::trajectory::orbit::Model;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::Propagated;
    using ostk::astro::trajectory::orbit::models::SGP4;

    class_<Model>(
        aModule,
        "OrbitModel",
        R"doc(
            Base class for orbit models.

            Provides the interface for orbit models.

            Group:
                orbit
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
}
