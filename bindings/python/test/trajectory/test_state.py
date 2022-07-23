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

def construct_state():
    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    state:State = State(epoch, position, velocity)

    assert state is not None
    assert isinstance(state, State)
    assert state.is_defined()

    return state

################################################################################################################################################################








################################################################################################################################################################
