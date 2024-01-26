/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesBroker.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubset.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/NumericalSolver.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::math::geometry::d3::transformation::rotation::Quaternion;
    using ostk::math::object::Vector3d;

    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::coord::Frame;
    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::state::CoordinatesBroker;

    class_<State>(
        aModule,
        "State",
        R"doc(
            This class represents the physical state of an object.

            Group:
                state
        )doc"
    )

        .def(
            init<const Instant&, const Position&, const Velocity&>(),
            R"doc(
                 Utility constructor for Position/Velocity only.
                 
                 Args:
                     instant (Instant): An instant
                     position (Position): The cartesian position at the instant
                     velocity (Velocity): The cartesian velocity at the instant
             )doc",
            arg("instant"),
            arg("position"),
            arg("velocity")
        )
        .def(
            init<
                const Instant&,
                const Position&,
                const Velocity&,
                const Quaternion&,
                const Vector3d&,
                const Shared<const Frame>&>(),
            R"doc(
                 Utility constructor for Position/Velocity/Attitude/Angular velocity.
                 
                 Args:
                     instant (Instant): An instant
                     position (Position): The cartesian position at the instant
                     velocity (Velocity): The cartesian velocity at the instant
                     attitude (Quaternion): The attitude at the instant, representing the rotation required to go from the attitude reference frame to the satellite body frame
                     angular_velocity (numpy.ndarray): The angular velocity at the instant, representing the angular velocity of the satellite body frame with respect ot teh attitude frame, expressed in body frame
                     attitude_frame (Frame): The attitude reference frame
             )doc",
            arg("instant"),
            arg("position"),
            arg("velocity"),
            arg("attitude"),
            arg("angular_velocity"),
            arg("attitude_frame")
        )
        .def(
            init<const Instant&, const VectorXd&, const Shared<const Frame>&, const Shared<const CoordinatesBroker>&>(),
            R"doc(
                 Constructor with a pre-defined Coordinates Broker.
                 
                 Args:
                     instant (Instant): An instant
                     coordinates (numpy.ndarray): The coordinates at the instant in International System of Units
                     frame (Frame): The reference frame in which the coordinates are referenced to and resolved in
                     coordinates_broker (CoordinatesBroker): The coordinates broker associated to the coordinates
             )doc",
            arg("instant"),
            arg("coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(
            init<
                const Instant&,
                const VectorXd&,
                const Shared<const Frame>&,
                const Array<Shared<const CoordinatesSubset>>&>(),
            R"doc(
                 Constructor with coordinate subsets.
                 
                 Args:
                     instant (Instant): An instant
                     coordinates (numpy.ndarray): The coordinates at the instant in International System of Units
                     frame (Frame): The reference frame in which the coordinates are referenced to and resolved in
                     coordinates_subsets (CoordinatesBroker): The coordinates subsets associated to the coordinates
             )doc",
            arg("instant"),
            arg("coordinates"),
            arg("frame"),
            arg("coordinates_subsets")
        )
        .def(init<const State&>(), arg("state"))

        .def(self == self)
        .def(self != self)
        .def(self + self)
        .def(self - self)

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def(
            "is_defined",
            &State::isDefined,
            R"doc(
                Check if the state is defined.

                Returns:
                    bool: True if the state is defined, False otherwise.
            )doc"
        )
        .def(
            "get_size",
            &State::getSize,
            R"doc(
                Get the size of the state.

                Returns:
                    int: The size of the state.
            )doc"
        )
        .def(
            "get_instant",
            &State::getInstant,
            R"doc(
                Get the instant of the state.

                Returns:
                    Instant: The instant of the state.
            )doc"
        )
        .def(
            "get_position",
            &State::getPosition,
            R"doc(
                Get the position of the state.

                Returns:
                    Position: The position of the state.
            )doc"
        )
        .def(
            "get_velocity",
            &State::getVelocity,
            R"doc(
                Get the velocity of the state.

                Returns:
                    Velocity: The velocity of the state.
            )doc"
        )
        .def(
            "get_coordinates",
            &State::getCoordinates,
            R"doc(
                Get the coordinates of the state.

                Returns:
                    np.array: The coordinates of the state.
            )doc"
        )
        .def(
            "get_coordinates_subsets",
            &State::getCoordinatesSubsets,
            R"doc(
                Get the coordinates subsets associated to the state.

                Returns:
                    list[CoordinatesSubset]: The coordinates subsets associated to the state.
            )doc"
        )
        .def(
            "get_frame",
            &State::getFrame,
            R"doc(
                Get the reference frame of the state.

                Returns:
                    Frame: The reference frame of the state.
            )doc"
        )

        .def(
            "has_subset",
            &State::hasSubset,
            R"doc(
                Check if the state has a given subset.

                Args:
                    subset (CoordinatesSubset): The subset to check.

                Returns:
                    bool: True if the state has the subset, False otherwise.
            )doc",
            arg("subset")
        )
        .def(
            "extract_coordinate",
            &State::extractCoordinate,
            R"doc(
                Extract the coordinates associated to a subset of the state.

                Args:
                    coordinates_subset (CoordinatesSubset): The coordinates subset to extract.

                Returns:
                    np.array: The coordinates associated to the subset.
            )doc",
            arg("coordinates_subset")
        )
        .def(
            "extract_coordinates",
            &State::extractCoordinates,
            R"doc(
                Extract the coordinates associated to a set of subsets of the state.

                Args:
                    coordinates_subsets (list[CoordinatesSubset]): The coordinates subsets to extract.

                Returns:
                    np.array: The coordinates associated to the subsets.
            )doc",
            arg("coordinates_subsets")
        )

        .def(
            "in_frame",
            &State::inFrame,
            R"doc(
                Check if the state is in a given reference frame.

                Args:
                    frame (Frame): The reference frame to check.

                Returns:
                    bool: True if the state is in the reference frame, False otherwise.
            )doc",
            arg("frame")
        )

        .def_static(
            "undefined",
            &State::Undefined,
            R"doc(
                Get an undefined state.

                Returns:
                    State: An undefined state.
            )doc"
        )

        ;

    // Create "state" python submodule
    auto state = aModule.def_submodule("state");

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesBroker(state);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubset(state);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_NumericalSolver(state);
}
