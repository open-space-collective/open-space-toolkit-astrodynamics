################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/profile/test_state.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.physics as physics

import ostk.mathematics as mathematics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Quaternion = mathematics.geometry.d3.transformations.rotations.Quaternion
DateTime = physics.time.DateTime
Instant = physics.time.Instant
Scale = physics.time.Scale
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
State = astrodynamics.flight.profile.State

################################################################################################################################################################

@pytest.fixture
def instant () -> Instant:

    return Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

@pytest.fixture
def frame () -> Frame:

    return Frame.GCRF()

@pytest.fixture
def position (frame: Frame) -> Position:

    return Position.meters([0.0, 3.2, 3000.0], frame)

@pytest.fixture
def velocity (frame: Frame) -> Velocity:

    return Velocity.meters_per_second([1.0, 3.0, 0.0], frame)

@pytest.fixture
def quaternion () -> Quaternion:

    return Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

@pytest.fixture
def angular_velocity () -> np.array:

    return np.array((0.01, 0.01, 0.0))

@pytest.fixture
def state (instant: Instant,
           position: Position,
           velocity: Velocity,
           quaternion: Quaternion,
           angular_velocity: np.array,
           frame: Frame) -> State:

    return State(
        instant,
        position,
        velocity,
        quaternion,
        angular_velocity,
        frame,
    )

################################################################################################################################################################

class TestState:

    def test_constructors (self):

        # Construct Instant
        instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

        # Construct Quaternion
        quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

        # Construct Frame
        frame: Frame = Frame.GCRF()

        # Construct State using lists
        position = Position.meters([0.0, 3.2, 3000.0], Frame.GCRF())
        velocity = Velocity.meters_per_second([1.0, 3.0, 0.0], Frame.GCRF())
        angular_velocity = [0.01, 0.01, 0.0]

        state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

        assert isinstance(state, State)

        # Construct State using tuples
        position = Position.meters((0.0, 3.2, 3000.0), Frame.GCRF())
        velocity = Velocity.meters_per_second((1.0, 3.0, 0.0), Frame.GCRF())
        angular_velocity = (0.01, 0.01, 0.0)

        state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

        assert isinstance(state, State)

        # Construct State using ndarray
        # position = Position.meters(np.array(position), Frame.GCRF())
        # velocity = Velocity.meters_per_second(np.array(velocity), Frame.GCRF())
        # angular_velocity = np.array(angular_velocity)

        # state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

        # assert isinstance(state, State)

    def test_comparators (self, state: State):

        assert (state == state) is True
        assert (state != state) is False

    def test_get_instant (self, instant: Instant, state: State):

        assert isinstance(state.get_instant(), Instant)
        assert state.get_instant() == instant

    def test_get_position (self, position: Position, state: State):

        assert isinstance(state.get_position(), Position)
        assert state.get_position() == position

    def test_get_velocity (self, velocity: Velocity, state: State):

        assert isinstance(state.get_velocity(), Velocity)
        assert state.get_velocity() == velocity

    def test_get_attitude (self, quaternion: Quaternion, state: State):

        assert isinstance(state.get_attitude(), Quaternion)
        assert state.get_attitude() == quaternion

    def test_get_angular_velocity (self, angular_velocity: np.array, state: State):

        assert isinstance(state.get_angular_velocity(), np.ndarray)
        assert np.array_equal(state.get_angular_velocity(), angular_velocity)

    def test_get_frame (self, frame: Frame, state: State):

        assert isinstance(state.get_frame(), Frame)
        assert state.get_frame() == frame

    def test_in_frame (self, frame: Frame, state: State):

        assert isinstance(state.in_frame(frame), State)

    def test_undefined (self):

        state: State = State.undefined()

        assert isinstance(state, State)
        assert state.is_defined() is False

################################################################################################################################################################
