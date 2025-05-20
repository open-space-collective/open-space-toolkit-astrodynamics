/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::astrodynamics::trajectory::Model;
    using ostk::astrodynamics::trajectory::model::Tabulated;
    using ostk::astrodynamics::trajectory::State;

    class_<Tabulated, Model>(
        aModule,
        "Tabulated",
        R"doc(
            A trajectory model defined by a set of states.

        )doc"
    )

        .def(
            init<const Array<State>&, const Interpolator::Type&>(),
            R"doc(
                Constructor.

                Args:
                    states (Array[State]): The states of the model.
                    interpolation_type (Interpolator.Type): The type of interpolation to use. Defaults to Linear.
             )doc",
            arg("states"),
            arg("interpolation_type") = Interpolator::Type::Linear
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
            "get_interpolation_type",
            &Tabulated::getInterpolationType,
            R"doc(
                Get the interpolation type of the model.

                Returns:
                    Interpolator.Type: The interpolation type of the model.
             )doc"
        )

        .def(
            "get_first_state",
            &Tabulated::getFirstState,
            R"doc(
                Get the first state of the model.

                Returns:
                    State: The first state of the model.
             )doc"
        )

        .def(
            "get_last_state",
            &Tabulated::getLastState,
            R"doc(
                Get the last state of the model.

                Returns:
                    State: The last state of the model.
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
            "calculate_states_at",
            &Tabulated::calculateStatesAt,
            R"doc(
                Calculate the states of the model at the specified instants.

                Args:
                    instants (list[Instant]): The instants at which to calculate the states.

                Returns:
                    list[State]: The states of the model at the specified instants.
             )doc",
            arg("instants")
        )

        ;
}
