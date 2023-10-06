# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinatesBroker
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


@pytest.fixture()
def position(frame: Frame) -> Position:
    return Position.meters([6371000.0, 0.0, 0.0], frame)


@pytest.fixture()
def velocity(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)


@pytest.fixture
def state(
    instant: Instant, position: Position, velocity: Velocity, frame: Frame
) -> State:
    return State(instant, position, velocity)


@pytest.fixture
def coordinates_broker() -> CoordinatesBroker:
    return CoordinatesBroker([CartesianPosition.default(), CartesianVelocity.default()])


class TestState:
    def test_constructor(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
    ):
        state = State(instant, position, velocity)
        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_explicit_constructor(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        state = State(instant, position, velocity)
        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

        state = State(
            instant,
            np.append(position.get_coordinates(), velocity.get_coordinates()),
            frame,
            coordinates_broker,
        )

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_comparators(self, state: State):
        assert (state == state) is True
        assert (state != state) is False

    def test_operators(self, state: State):
        assert isinstance(state + state, State)
        assert isinstance(state - state, State)

    def test_getters(
        self,
        state: State,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        assert state.get_instant() == instant
        assert state.get_position() == position
        assert state.get_velocity() == velocity
        assert state.get_frame() == frame
        assert (
            state.get_coordinates()
            == np.append(position.get_coordinates(), velocity.get_coordinates())
        ).all()
        assert state.get_coordinates_subsets() == coordinates_broker.get_subsets()

    def test_in_frame(
        self,
        state: State,
        frame: Frame,
    ):
        assert state.in_frame(frame) == state
        assert state.in_frame(Frame.ITRF()) != state

    def test_extract_coordinates(
        self,
        state: State,
    ):
        position_coordinates = state.extract_coordinates(CartesianPosition.default())
        velocity_coordinates = state.extract_coordinates(CartesianVelocity.default())

        assert len(position_coordinates) == 3
        assert len(velocity_coordinates) == 3
        assert (position_coordinates == state.get_position().get_coordinates()).all()
        assert (velocity_coordinates == state.get_velocity().get_coordinates()).all()

        pv_coordinates = state.extract_coordinates(
            [CartesianPosition.default(), CartesianVelocity.default()]
        )
        assert len(pv_coordinates) == 6
        assert (pv_coordinates == state.get_coordinates()).all()
