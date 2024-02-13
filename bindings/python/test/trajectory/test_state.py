# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateBroker
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


@pytest.fixture()
def position(frame: Frame) -> Position:
    return Position.meters([6371000.0, 0.0, 0.0], frame)


@pytest.fixture()
def velocity(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)


@pytest.fixture()
def attitude() -> Quaternion:
    return Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)


@pytest.fixture()
def angular_velocity() -> np.ndarray:
    return np.array([-1.0, -2.0, -3.0])


@pytest.fixture
def state(
    instant: Instant,
    position: Position,
    velocity: Velocity,
) -> State:
    return State(instant, position, velocity)


@pytest.fixture
def profile_state(
    instant: Instant,
    position: Position,
    velocity: Velocity,
    attitude: Quaternion,
    angular_velocity: np.ndarray,
    frame: Frame,
) -> State:
    return State(instant, position, velocity, attitude, angular_velocity, frame)


@pytest.fixture
def coordinate_broker() -> CoordinateBroker:
    return CoordinateBroker([CartesianPosition.default(), CartesianVelocity.default()])


class TestState:
    def test_constructor_position_velocity(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
    ):
        state = State(instant, position, velocity)
        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_constructor_position_velocity_attitude_angular_velocity(
        self,
        instant: Instant,
        frame: Frame,
        position: Position,
        velocity: Velocity,
        attitude: Quaternion,
        angular_velocity: np.ndarray,
    ):
        state = State(instant, position, velocity, attitude, angular_velocity, frame)
        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_explicit_constructor(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        frame: Frame,
        coordinate_broker: CoordinateBroker,
    ):
        state = State(instant, position, velocity)
        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

        state = State(
            instant,
            np.append(position.get_coordinates(), velocity.get_coordinates()),
            frame,
            coordinate_broker,
        )

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_subsets_constructor(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        frame: Frame,
    ):
        state = State(
            instant,
            np.append(position.get_coordinates(), velocity.get_coordinates()),
            frame,
            [CartesianPosition.default(), CartesianVelocity.default()],
        )

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_custom_state_class_generator(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        frame: Frame,
    ):
        state = State(
            instant,
            np.append(position.get_coordinates(), velocity.get_coordinates()),
            frame,
            [CartesianPosition.default(), CartesianVelocity.default()],
        )

        MySuperFunState: type = State.template(
            frame,
            [CartesianPosition.default(), CartesianVelocity.default()],
        )

        custom_state: MySuperFunState = MySuperFunState(
            instant,
            np.append(position.get_coordinates(), velocity.get_coordinates()),
        )

        assert custom_state is not None
        assert isinstance(custom_state, MySuperFunState)
        assert isinstance(state, State)
        assert custom_state.is_defined()

        assert custom_state == state
        assert custom_state is not state

    def test_comparators(self, state: State):
        assert (state == state) is True
        assert (state != state) is False

    def test_operators(self, state: State):
        assert isinstance(state + state, State)
        assert isinstance(state - state, State)

    def test_getters(
        self,
        profile_state: State,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        attitude: Quaternion,
        angular_velocity: np.ndarray,
        frame: Frame,
    ):
        assert profile_state.get_instant() == instant
        assert profile_state.get_position() == position
        assert profile_state.get_velocity() == velocity
        assert profile_state.get_attitude() == attitude
        assert np.all(profile_state.get_angular_velocity() == angular_velocity)
        assert profile_state.has_subset(CartesianPosition.default())
        assert profile_state.has_subset(CartesianVelocity.default())
        assert profile_state.get_frame() == frame
        assert profile_state.get_coordinates() is not None
        assert profile_state.get_coordinate_subsets() is not None

    def test_in_frame(
        self,
        state: State,
        frame: Frame,
    ):
        assert state.in_frame(frame) == state
        assert state.in_frame(Frame.ITRF()) != state

    def test_extract_coordinate(
        self,
        state: State,
    ):
        position_coordinates = state.extract_coordinate(CartesianPosition.default())
        velocity_coordinates = state.extract_coordinate(CartesianVelocity.default())

        assert len(position_coordinates) == 3
        assert len(velocity_coordinates) == 3
        assert (position_coordinates == state.get_position().get_coordinates()).all()
        assert (velocity_coordinates == state.get_velocity().get_coordinates()).all()

    def test_extract_coordinates(
        self,
        state: State,
    ):
        pv_coordinates = state.extract_coordinates(
            [CartesianPosition.default(), CartesianVelocity.default()]
        )
        assert len(pv_coordinates) == 6
        assert (pv_coordinates == state.get_coordinates()).all()
