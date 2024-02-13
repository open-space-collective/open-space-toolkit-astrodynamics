/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_StateBuilder(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::container::Array;

    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::StateBuilder;
    using ostk::astrodynamics::trajectory::state::CoordinateBroker;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<StateBuilder>(
        aModule,
        "StateBuilder",
        R"doc(
            This class makes it convenient to build a `State` object.
        
        )doc"
    )

        .def(
            init<const Shared<const Frame>&, const Array<Shared<const CoordinateSubset>>&>(),
            arg("frame"),
            arg("coordinate_subsets"),
            R"doc(
                Construct a new `StateBuilder` object.

                Arguments:
                    frame (Frame): The reference frame.
                    coordinate_subsets list[CoordinateSubset]: The coordinate subsets.

                Returns:
                    StateBuilder 

            )doc"
        )
        .def(
            init<const Shared<const Frame>&, const Shared<const CoordinateBroker>&>(),
            arg("frame"),
            arg("coordinate_broker"),
            R"doc(
                Construct a new `StateBuilder` object.

                Arguments:
                    frame (Frame): The reference frame.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    StateBuilder: The new `StateBuilder` object.

            )doc"
        )
        .def(
            init<const State&>(),
            arg("state"),
            R"doc(
                Construct a new `StateBuilder` object.

                Arguments:
                    state (State): The state.

                Returns:
                    StateBuilder: The new `StateBuilder` object.
                
            )doc"
        )

        .def(
            self == self,
            R"doc(
                Check if two `StateBuilder` objects are equal.

                Returns:
                    bool: True if the two `StateBuilder` objects are equal, False otherwise.
            
            )doc"
        )
        .def(
            self != self,
            R"doc(
                Check if two `StateBuilder` objects are not equal.

                Returns:
                    bool: True if the two `StateBuilder` objects are not equal, False otherwise.

            )doc"
        )
        .def(
            "__add__",
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr)
            {
                return aStateBuilder + aCoordinateSubsetSPtr;
            },
            is_operator(),
            R"doc(
                Add a coordinate subset to the `StateBuilder`.

                Arguments:
                    coordinate_subsets (CoordinateSubset): The coordinate subset to add.

                Returns:
                    StateBuilder: The `StateBuilder` with the added coordinate subset.

            )doc"
        )
        .def(
            "__sub__",
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr)
            {
                return aStateBuilder - aCoordinateSubsetSPtr;
            },
            is_operator(),
            R"doc(
                Remove a coordinate subset from the `StateBuilder`.

                Arguments:
                    coordinate_subset (CoordinateSubset): The coordinate subset to remove.

                Returns:
                    StateBuilder: The `StateBuilder` with the removed coordinate subset.

            )doc"
        )

        .def("__str__", &(shiftToString<StateBuilder>))
        .def("__repr__", &(shiftToString<StateBuilder>))

        .def(
            "is_defined",
            &StateBuilder::isDefined,
            R"doc(
                Check if the `StateBuilder` is defined.

                Returns:
                    bool: True if the `StateBuilder` is defined, False otherwise.

            )doc"
        )

        .def(
            "build",
            &StateBuilder::build,
            arg("instant"),
            arg("coordinates"),
            R"doc(
                Build a `State` object from the `StateBuilder`.

                Arguments:
                    instant (Instant): The instant of the state.
                    coordinates (VectorXd): The coordinates of the state.

                Returns:
                    State: The `State` object built from the `StateBuilder`.

            )doc"
        )
        .def(
            "reduce",
            &StateBuilder::reduce,
            arg("state"),
            R"doc(
                Reduce a `State` object to the `StateBuilder`.

                Arguments:
                    state (State): The `State` object to reduce.

                Returns:
                    StateBuilder: The `StateBuilder` object reduced from the `State`.

            )doc"
        )
        .def(
            "expand",
            &StateBuilder::expand,
            arg("state"),
            arg("default_state"),
            R"doc(
                Expand a `State` object to the `StateBuilder`.

                Arguments:
                    state (State): The `State` object to expand.
                    default_state (State): The default `State` object.

                Returns:
                    StateBuilder: The `StateBuilder` object expanded from the `State`.

            )doc"
        )

        .def(
            "get_coordinate_subsets",
            &StateBuilder::getCoordinateSubsets,
            R"doc(
                Get the coordinate subsets of the `StateBuilder`.

                Returns:
                    Array<Shared<const CoordinateSubset>>: The coordinate subsets of the `StateBuilder`.

            )doc"
        )
        .def(
            "access_coordinate_broker",
            &StateBuilder::accessCoordinateBroker,
            R"doc(
                Access the coordinate broker of the `StateBuilder`.

                Returns:
                    CoordinateBroker: The coordinate broker of the `StateBuilder`.

            )doc"
        )
        .def(
            "get_frame",
            &StateBuilder::getFrame,
            R"doc(
                Get the reference frame of the `StateBuilder`.

                Returns:
                    Frame: The reference frame of the `StateBuilder`.

            )doc"
        )

        .def_static(
            "undefined",
            &StateBuilder::Undefined,
            R"doc(
                Get an undefined `StateBuilder`.

                Returns:
                    StateBuilder: The undefined `StateBuilder`.
            )doc"
        )

        ;
}
