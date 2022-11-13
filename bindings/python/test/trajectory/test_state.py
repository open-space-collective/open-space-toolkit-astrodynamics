################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/test_state.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State

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

    def test_constructor (self, state: State):

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()

    def test_comparators (self, state: State):

        assert (state == state) is True
        assert (state != state) is False

    def test_operators (self, state: State):

        assert isinstance(state + state, State)
        assert isinstance(state - state, State)

    def test_getters (self, state: State, state_default_inputs):

        (instant, position, velocity, frame) = state_default_inputs

        assert state.get_instant() == instant
        assert state.get_position() == position
        assert state.get_velocity() == velocity
        assert (state.get_coordinates() == np.append(position.get_coordinates(), velocity.get_coordinates())).all()

    def test_in_frame (self, state: State, state_default_inputs):

        (_, _, _, frame) = state_default_inputs

        assert state.in_frame(frame) == state
        assert state.in_frame(Frame.ITRF()) != state

################################################################################################################################################################
