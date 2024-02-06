/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

using namespace pybind11;

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::models::Tabulated;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Tabulated(pybind11::module& aModule)
{
    class_<Tabulated, ostk::astro::trajectory::orbit::Model> tabulated_class(
        aModule,
        "Tabulated",
        R"doc(
            Tabulated orbit model.

        )doc"
    );

    enum_<Tabulated::InterpolationType>(
        tabulated_class,
        "InterpolationType",
        R"doc(
            The Interpolation Type.
        )doc"
    )
        .value("Linear", Tabulated::InterpolationType::Linear, "Linear")
        .value("CubicSpline", Tabulated::InterpolationType::CubicSpline, "Cubic Spline")
        .value("BarycentricRational", Tabulated::InterpolationType::BarycentricRational, "Barycentric Rational")

        ;

    tabulated_class

        .def(
            init<Array<State>, Integer, Tabulated::InterpolationType>(),
            R"doc(
                Constructor.

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    interpolation_type (Tabulated.InterpolationType, optional): The interpolation type.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("interpolation_type") = DEFAULT_TABULATED_INTERPOLATION_TYPE
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
                    Tabulated.InterpolationType: The interpolation type.

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
