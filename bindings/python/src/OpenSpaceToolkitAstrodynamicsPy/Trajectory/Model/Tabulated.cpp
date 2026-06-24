/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Map;
    using ostk::core::type::Shared;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::trajectory::Model;
    using ostk::astrodynamics::trajectory::model::Tabulated;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

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
                    states (list[State]): The states of the model.
                    interpolation_type (Interpolator.Type): The type of interpolation to use. Defaults to Linear.
            )doc",
            arg("states"),
            arg_v("interpolation_type", Interpolator::Type::Linear, "Interpolator.Type.Linear")
        )

        .def(
            init<const Array<State>&, const Interpolator::Type&, const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Args:
                    states (list[State]): The states of the model.
                    interpolation_type (Interpolator.Type): The type of interpolation to use.
                    output_frame (Frame): The reference frame in which the computed states are expressed.
             )doc",
            arg("states"),
            arg("interpolation_type"),
            arg("output_frame")
        )

        .def(
            init<const Array<State>&, const Map<Shared<const CoordinateSubset>, Interpolator::Type>&>(),
            R"doc(
                Constructor with per-coordinate-subset interpolation types.

                Each coordinate is interpolated using the interpolation type associated with the coordinate subset
                it belongs to.

                Args:
                    states (list[State]): The states of the model.
                    interpolation_types (dict[CoordinateSubset, Interpolator.Type]): A mapping from coordinate subset to the interpolation type to use for that subset's coordinates. Every coordinate subset present in the states must have an entry, and every coordinate subset in the map must be present in the states.
             )doc",
            arg("states"),
            arg("interpolation_types")
        )

        .def(
            init<
                const Array<State>&,
                const Map<Shared<const CoordinateSubset>, Interpolator::Type>&,
                const Shared<const Frame>&>(),
            R"doc(
                Constructor with per-coordinate-subset interpolation types.

                Each coordinate is interpolated using the interpolation type associated with the coordinate subset
                it belongs to.

                Args:
                    states (list[State]): The states of the model.
                    interpolation_types (dict[CoordinateSubset, Interpolator.Type]): A mapping from coordinate subset to the interpolation type to use for that subset's coordinates. Every coordinate subset present in the states must have an entry, and every coordinate subset in the map must be present in the states.
                    output_frame (Frame): The reference frame in which the computed states are expressed.
             )doc",
            arg("states"),
            arg("interpolation_types"),
            arg("output_frame")
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
            "get_frame",
            &Tabulated::getFrame,
            R"doc(
                Get the reference frame in which the computed states are expressed.

                Returns:
                    Frame: The output reference frame.
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

        .def_static(
            "default",
            overload_cast<const Array<State>&>(&Tabulated::Default),
            R"doc(
                Construct a tabulated model using the default per-coordinate-subset interpolation types.

                Each coordinate subset present in the states is interpolated using its default interpolation type
                (barycentric rational for position, velocity, acceleration, attitude, angular velocity and mass;
                zero-order for drag coefficient, surface area, mass flow rate and ballistic coefficient).

                Args:
                    states (list[State]): The states of the model.

                Returns:
                    Tabulated: A tabulated model using the default interpolation types.
             )doc",
            arg("states")
        )

        .def_static(
            "default",
            overload_cast<const Array<State>&, const Shared<const Frame>&>(&Tabulated::Default),
            R"doc(
                Construct a tabulated model using the default per-coordinate-subset interpolation types.

                Each coordinate subset present in the states is interpolated using its default interpolation type
                (barycentric rational for position, velocity, acceleration, attitude, angular velocity and mass;
                zero-order for drag coefficient, surface area, mass flow rate and ballistic coefficient).

                Args:
                    states (list[State]): The states of the model.
                    output_frame (Frame): The reference frame in which the computed states are expressed.

                Returns:
                    Tabulated: A tabulated model using the default interpolation types.
             )doc",
            arg("states"),
            arg("output_frame")
        )

        ;
}
