/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::type::Integer;
using ostk::core::type::Shared;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

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
            arg_v(
                "interpolation_type",
                DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE,
                "Interpolator.Type.BarycentricRational"
            )
        )

        .def(
            init<const Array<State>&, const Integer&, const Interpolator::Type&, const Shared<const Frame>&>(),
            R"doc(
                Constructor with an explicit output frame.

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    interpolation_type (Interpolator.Type): The interpolation type.
                    output_frame (Frame): The reference frame in which the computed states are expressed. The provided states are converted to this frame and interpolation is performed in this frame.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("interpolation_type"),
            arg("output_frame")
        )

        .def(
            init<const Array<State>&, const Integer&, const Map<Shared<const CoordinateSubset>, Interpolator::Type>&>(),
            R"doc(
                Constructor with per-coordinate-subset interpolation types.

                Each coordinate is interpolated using the interpolation type associated with the coordinate subset
                it belongs to.

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    interpolation_types (dict[CoordinateSubset, Interpolator.Type]): A mapping from coordinate subset to the interpolation type to use for that subset's coordinates. Every coordinate subset present in the states must have an entry, and every coordinate subset in the map must be present in the states.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("interpolation_types")
        )

        .def(
            init<
                const Array<State>&,
                const Integer&,
                const Map<Shared<const CoordinateSubset>, Interpolator::Type>&,
                const Shared<const Frame>&>(),
            R"doc(
                Constructor with per-coordinate-subset interpolation types and an explicit output frame.

                Each coordinate is interpolated using the interpolation type associated with the coordinate subset
                it belongs to.

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    interpolation_types (dict[CoordinateSubset, Interpolator.Type]): A mapping from coordinate subset to the interpolation type to use for that subset's coordinates. Every coordinate subset present in the states must have an entry, and every coordinate subset in the map must be present in the states.
                    output_frame (Frame): The reference frame in which the computed states are expressed. The provided states are converted to this frame and interpolation is performed in this frame.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("interpolation_types"),
            arg("output_frame")
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

        .def_static(
            "default",
            overload_cast<const Array<State>&, const Integer&>(&Tabulated::Default),
            R"doc(
                Construct a tabulated orbit model using the default per-coordinate-subset interpolation types.

                Each coordinate subset present in the states is interpolated using its default interpolation type
                (barycentric rational for position, velocity, acceleration, attitude, angular velocity and mass;
                zero-order for drag coefficient, surface area, mass flow rate and ballistic coefficient).

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int, optional): The initial revolution number. Defaults to 1.

                Returns:
                    Tabulated: A tabulated orbit model using the default interpolation types.

            )doc",
            arg("states"),
            arg_v("initial_revolution_number", Integer(1), "1")
        )

        .def_static(
            "default",
            overload_cast<const Array<State>&, const Integer&, const Shared<const Frame>&>(&Tabulated::Default),
            R"doc(
                Construct a tabulated orbit model using the default per-coordinate-subset interpolation types and an
                explicit output frame.

                Each coordinate subset present in the states is interpolated using its default interpolation type
                (barycentric rational for position, velocity, acceleration, attitude, angular velocity and mass;
                zero-order for drag coefficient, surface area, mass flow rate and ballistic coefficient).

                Args:
                    states (list[State]): The states.
                    initial_revolution_number (int): The initial revolution number.
                    output_frame (Frame): The reference frame in which the computed states are expressed. The provided states are converted to this frame and interpolation is performed in this frame.

                Returns:
                    Tabulated: A tabulated orbit model using the default interpolation types.

            )doc",
            arg("states"),
            arg("initial_revolution_number"),
            arg("output_frame")
        )

        ;
}
