################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/test_state.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import ostk.physics as physics
import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Instant = physics.time.Instant
DateTime = physics.time.DateTime
Time = physics.units.Time
Scale = physics.time.Scale
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame

State = astrodynamics.trajectory.State

################################################################################################################################################################

@pytest.fixture
def state_default_inputs ():

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    return (instant, position, velocity, frame)

@pytest.fixture
def state (state_default_inputs) -> State:

    return State(*state_default_inputs[0:-1])

################################################################################################################################################################

class TestState:

    def test_constructors(self, state: State):

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_comparators (self, state: State):

        assert (state == state) is True
        assert (state != state) is False

    def test_getters (self, state: State, state_default_inputs):

        (instant, position, velocity, frame) = state_default_inputs

        assert state.get_instant() == instant
        assert state.get_position() == position
        assert state.get_velocity() == velocity

    def test_in_frame (self, state: State, state_default_inputs):

        (instant, position, velocity, frame) = state_default_inputs

        assert state.in_frame(frame) == state
        assert state.in_frame(Frame.ITRF()) != state

################################################################################################################################################################
