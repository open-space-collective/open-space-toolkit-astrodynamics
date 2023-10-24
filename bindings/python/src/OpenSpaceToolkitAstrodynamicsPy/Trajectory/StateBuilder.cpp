/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_StateBuilder(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::physics::coord::Frame;

    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::StateBuilder;
    using ostk::astro::trajectory::state::CoordinatesBroker;
    using ostk::astro::trajectory::state::CoordinatesSubset;

    class_<StateBuilder>(
        aModule,
        "StateBuilder",
        R"doc(
            This class makes it convenient to build a `State` object.
        
            Group:
                trajectory
        )doc"
    )

        .def(
            init<const Shared<const Frame>&, const Array<Shared<const CoordinatesSubset>>&>(),
            arg("frame"),
            arg("coordinates_subsets"),
            R"pydoc(
                Construct a new `StateBuilder` object.

                Arguments:
                    frame (Frame): The reference frame.
                    coordinates_subsets list[CoordinatesSubset]: The coordinates subsets.

                Returns:
                    StateBuilder 

            )pydoc"
        )
        .def(
            init<const Shared<const Frame>&, const Shared<const CoordinatesBroker>&>(),
            arg("frame"),
            arg("coordinates_broker"),
            R"doc(
                Construct a new `StateBuilder` object.

                Arguments:
                    frame (Frame): The reference frame.
                    coordinates_broker (CoordinatesBroker): The coordinates broker.

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
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
            {
                return aStateBuilder + aCoordinatesSubsetSPtr;
            },
            is_operator(),
            R"doc(
                Add a coordinates subset to the `StateBuilder`.

                Arguments:
                    coordinates_subsets (CoordinatesSubset): The coordinates subset to add.

                Returns:
                    StateBuilder: The `StateBuilder` with the added coordinates subset.

            )doc"
        )
        .def(
            "__sub__",
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
            {
                return aStateBuilder - aCoordinatesSubsetSPtr;
            },
            is_operator(),
            R"doc(
                Remove a coordinates subset from the `StateBuilder`.

                Arguments:
                    coordinates_subset (CoordinatesSubset): The coordinates subset to remove.

                Returns:
                    StateBuilder: The `StateBuilder` with the removed coordinates subset.

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
            "get_coordinates_subsets",
            &StateBuilder::getCoordinatesSubsets,
            R"doc(
                Get the coordinates subsets of the `StateBuilder`.

                Returns:
                    Array<Shared<const CoordinatesSubset>>: The coordinates subsets of the `StateBuilder`.

            )doc"
        )
        .def(
            "access_coordinates_broker",
            &StateBuilder::accessCoordinatesBroker,
            R"doc(
                Access the coordinates broker of the `StateBuilder`.

                Returns:
                    CoordinatesBroker: The coordinates broker of the `StateBuilder`.

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
