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
def state_default_inputs_fix ():

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    return instant, position, velocity, frame

@pytest.fixture
def state_fix (state_default_inputs_fix) -> State:
    instant, position, velocity, frame = state_default_inputs_fix
    state: State = State(instant, position, velocity)

    return state

################################################################################################################################################################

class TestState:
    def test_constructors(self, state_fix: State):

        assert state_fix is not None
        assert isinstance(state_fix, State)
        assert state_fix.is_defined()

    def test_comparators (self, state_fix: State):

        assert state_fix == state_fix
        assert (state_fix != state_fix) is False

    def test_getters (self, state_fix: State, state_default_inputs_fix):

        instant, position, velocity, frame = state_default_inputs_fix
        assert state_fix.get_instant() == instant
        assert state_fix.get_position() == position
        assert state_fix.get_velocity() == velocity

    def test_in_frame (self, state_fix: State, state_default_inputs_fix):

        instant, position, velocity, frame = state_default_inputs_fix
        assert state_fix.in_frame(frame) == state_fix
        assert state_fix.in_frame(Frame.ITRF()) != state_fix

################################################################################################################################################################
