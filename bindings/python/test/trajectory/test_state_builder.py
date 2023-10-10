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
    CoordinatesBroker,
    CoordinatesSubset,
)
from ostk.astrodynamics.trajectory.state.coordinates_subset import (
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
def coordinates_subsets() -> list[CoordinatesSubset]:
    return [CartesianPosition.default(), CartesianVelocity.default()]


@pytest.fixture
def coordinates_broker(coordinates_subsets: list[CoordinatesSubset]) -> CoordinatesBroker:
    return CoordinatesBroker(coordinates_subsets)


@pytest.fixture
def state_builder(frame: Frame, coordinates_broker: CoordinatesBroker) -> State:
    return StateBuilder(frame, coordinates_broker)


class TestState:
    def test_broker_constructor(
        self,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        builder = StateBuilder(frame, coordinates_broker)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_subsets_constructor(
        self,
        frame: Frame,
        coordinates_subsets: list[CoordinatesSubset],
    ):
        builder = StateBuilder(frame, coordinates_subsets)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_comparators(self, state_builder: StateBuilder):
        assert (state_builder == state_builder) is True
        assert (state_builder != state_builder) is False

    def test_build_state(
        self,
        instant: Instant,
        state_builder: StateBuilder,
    ):
        coordinates = [1, 2, 3, 1, 2, 3]
        state: State = state_builder.state(instant, coordinates)

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()
        assert state.get_instant() == instant
        assert (state.get_coordinates() == coordinates).all()
        assert state.get_frame() == state_builder.get_frame()
        assert state.get_coordinates_subsets() == state_builder.get_coordinates_subsets()

    def test_getters(
        self,
        state_builder: StateBuilder,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        assert state_builder.get_frame() == frame
        assert state_builder.get_coordinates_subsets() == coordinates_broker.get_subsets()
