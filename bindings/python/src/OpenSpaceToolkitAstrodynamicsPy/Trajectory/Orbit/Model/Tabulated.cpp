/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::type::Integer;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
using ostk::astrodynamics::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Tabulated(pybind11::module& aModule)
{
    class_<Tabulated, ostk::astrodynamics::trajectory::orbit::Model>(
        aModule,
        "Tabulated",
        R"doc(
            Tabulated orbit model.

        )doc"
    )

        .def(
            init<Array<State>, Integer, Interpolator::Type>(),
            R"doc(
                Constructor.

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    interpolation_type (Interpolator.Type, optional): The interpolation type.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("interpolation_type") = DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE
        )

        .def(self == self)

        .def(self != self)

        .def("__str__", &(shiftToString<Tabulated>))

        .def("__repr__", &(shiftToString<Tabulated>))

        .def(
            "is_defined",
            &Tabulated::isDefined,
            R"doc(
                Check if the `Tabulated` model is defined.

                Returns:
                    bool: True if the `Tabulated` model is defined, False otherwise.

            )doc"
        )

        .def(
            "get_epoch",
            &Tabulated::getEpoch,
            R"doc(
                Get the epoch of the `Tabulated` model.

                Returns:
                    Instant: The epoch.

            )doc"
        )

        .def(
            "get_revolution_number_at_epoch",
            &Tabulated::getRevolutionNumberAtEpoch,
            R"doc(
                Get the revolution number at the epoch of the `Tabulated` model.

                Returns:
                    int: The revolution number.

            )doc"
        )

        .def(
            "get_interval",
            &Tabulated::getInterval,
            R"doc(
                Get the interval of the `Tabulated` model.

                Returns:
                    Interval: The interval.

            )doc"
        )

        .def(
            "get_interpolation_type",
            &Tabulated::getInterpolationType,
            R"doc(
                Get the interpolation type of the `Tabulated` model.

                Returns:
                    Interpolator.Type: The interpolation type.

            )doc"
        )

        .def(
            "calculate_state_at",
            &Tabulated::calculateStateAt,
            R"doc(
                Calculate the state of the `Tabulated` model at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state.

            )doc",
            arg("instant")
        )

        .def(
            "calculate_states_at",
            &Tabulated::calculateStatesAt,
            R"doc(
                Calculate the states of the `Tabulated` model at given instants.

                Args:
                    instants (list[Instant]): The instants.

                Returns:
                    list[State]: The states.

            )doc",
            arg("instants")
        )

        .def(
            "calculate_revolution_number_at",
            &Tabulated::calculateRevolutionNumberAt,
            R"doc(
                Calculate the revolution number of the `Tabulated` model at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    int: The revolution number.

            )doc",
            arg("instant")
        )

        ;
}
