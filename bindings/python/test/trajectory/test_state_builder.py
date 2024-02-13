# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import (
    State,
    StateBuilder,
)
from ostk.astrodynamics.trajectory.state import (
    CoordinateBroker,
    CoordinateSubset,
)
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)


@pytest.fixture()
def instant() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [CartesianPosition.default(), CartesianVelocity.default()]


@pytest.fixture
def coordinates() -> list[float]:
    return [1.0, 2.0, 3.0, 4.0, 5.0, 6.0]


@pytest.fixture
def coordinate_broker(
    coordinate_subsets: list[CoordinateSubset],
) -> CoordinateBroker:
    return CoordinateBroker(coordinate_subsets)


@pytest.fixture
def state(
    instant: Instant,
    coordinates: list[float],
    frame: Frame,
    coordinate_broker: CoordinateBroker,
) -> State:
    return State(instant, coordinates, frame, coordinate_broker)


@pytest.fixture
def state_builder(frame: Frame, coordinate_broker: CoordinateBroker) -> State:
    return StateBuilder(frame, coordinate_broker)


class TestStateBuilder:
    def test_broker_constructor(
        self,
        frame: Frame,
        coordinate_broker: CoordinateBroker,
    ):
        builder = StateBuilder(frame, coordinate_broker)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_subsets_constructor(
        self,
        frame: Frame,
        coordinate_subsets: list[CoordinateSubset],
    ):
        builder = StateBuilder(frame, coordinate_subsets)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_state_constructor(
        self,
        state: State,
    ):
        builder = StateBuilder(state)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_comparators(self, state_builder: StateBuilder):
        assert (state_builder == state_builder) is True
        assert (state_builder != state_builder) is False

    def test_operators(
        self,
        state_builder: StateBuilder,
    ):
        added_builder: StateBuilder = state_builder + CoordinateSubset.mass()
        assert isinstance(added_builder, StateBuilder)
        assert state_builder != added_builder

        subtracted_builder: StateBuilder = state_builder - CartesianPosition.default()
        assert isinstance(subtracted_builder, StateBuilder)
        assert state_builder != subtracted_builder

    def test_build(
        self,
        instant: Instant,
        state_builder: StateBuilder,
    ):
        coordinates = [1, 2, 3, 1, 2, 3]
        state: State = state_builder.build(instant, coordinates)

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()
        assert state.get_instant() == instant
        assert (state.get_coordinates() == coordinates).all()
        assert state.get_frame() == state_builder.get_frame()
        assert state.get_coordinate_subsets() == state_builder.get_coordinate_subsets()

    def test_reduce(
        self,
        state: State,
    ):
        builder = StateBuilder(state.get_frame(), [CartesianPosition.default()])
        reduced_state: State = builder.reduce(state)

        assert isinstance(reduced_state, State)
        assert state != reduced_state

    def test_expand(
        self,
        state: State,
    ):
        builder = StateBuilder(
            state.get_frame(),
            [
                CartesianPosition.default(),
                CartesianVelocity.default(),
                CoordinateSubset.mass(),
            ],
        )
        default_state: State = State(
            state.get_instant(),
            [100],
            state.get_frame(),
            CoordinateBroker([CoordinateSubset.mass()]),
        )
        expanded_state: State = builder.expand(state, default_state)

        assert isinstance(expanded_state, State)
        assert state != expanded_state
        assert default_state != expanded_state

    def test_getters(
        self,
        state_builder: StateBuilder,
        frame: Frame,
        coordinate_broker: CoordinateBroker,
    ):
        assert state_builder.get_frame() == frame
        assert state_builder.get_coordinate_subsets() == coordinate_broker.get_subsets()
