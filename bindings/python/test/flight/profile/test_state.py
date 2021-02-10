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
Frame = physics.coordinate.Frame
State = astrodynamics.flight.profile.State

################################################################################################################################################################

def test_flight_profile_state_constructors ():

    # Construct Instant
    instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

    # Construct Quaternion
    quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    # Construct Frame
    frame: Frame = Frame.GCRF()

    # Construct State using lists
    position = [0.0, 3.2, 3000.0]
    velocity = [1.0, 3.0, 0.0]
    angular_velocity = [0.01, 0.01, 0.0]

    state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

    assert state is not None
    assert isinstance(state, State)

    # Construct State using tuples
    position = (0.0, 3.2, 3000.0)
    velocity = (1.0, 3.0, 0.0)
    angular_velocity = (0.01, 0.01, 0.0)

    state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

    assert state is not None
    assert isinstance(state, State)

    # Construct State using ndarray
    position = np.array(position)
    velocity = np.array(velocity)
    angular_velocity = np.array(angular_velocity)

    state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

    assert state is not None
    assert isinstance(state, State)

################################################################################################################################################################

def test_flight_profile_state_comparators ():

    # Construct Instant
    instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

    # Construct Quaternion
    quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    # Construct Frame
    frame: Frame = Frame.GCRF()

    # Construct State using lists
    position_1 = [0.0, 3.2, 3000.0]
    position_2 = [3.0, 23.4, 3400.0]
    velocity = [1.0, 3.0, 0.0]
    angular_velocity = [0.01, 0.01, 0.0]

    state_1: State = State(instant, position_1, velocity, quaternion, angular_velocity, frame)
    state_2: State = State(instant, position_2, velocity, quaternion, angular_velocity, frame)

    assert state_1 == state_1
    assert state_2 == state_2
    assert state_1 != state_2

################################################################################################################################################################

def test_flight_profile_state_getters ():

    # Construct Instant
    instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

    # Construct Quaternion
    quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    # Construct Frame
    frame: Frame = Frame.GCRF()

    # Construct State using lists
    position = [0.0, 3.2, 3000.0]
    velocity = [1.0, 3.0, 0.0]
    angular_velocity = [0.01, 0.01, 0.0]

    state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

    # get_instant
    ans_instant: Instant = state.get_instant()

    assert ans_instant is not None
    assert isinstance(ans_instant, Instant)
    assert ans_instant == instant

    # get_position
    ans_position = state.get_position()

    assert ans_position is not None
    assert isinstance(ans_position, np.ndarray)
    assert np.array_equal(ans_position, position)

    # get_velocity
    ans_velocity = state.get_velocity()

    assert ans_velocity is not None
    assert isinstance(ans_velocity, np.ndarray)
    assert np.array_equal(ans_velocity, velocity)

    # get_angular_velocity
    ans_angular_velocity = state.get_angular_velocity()

    assert ans_angular_velocity is not None
    assert isinstance(ans_angular_velocity, np.ndarray)
    assert np.array_equal(ans_angular_velocity, angular_velocity)

    # get_attitude
    ans_attitude: Instant = state.get_attitude()

    assert ans_attitude is not None
    assert isinstance(ans_attitude, Quaternion)
    assert ans_attitude == quaternion

    # get_frame
    ans_frame: Frame = state.get_frame()

    assert ans_frame is not None
    assert isinstance(ans_frame, Frame)
    assert ans_frame == frame

################################################################################################################################################################

def test_flight_profile_state_in_frame ():

    # Construct Instant
    instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

    # Construct Quaternion
    quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    # Construct Frame
    frame: Frame = Frame.GCRF()

    # Construct State using lists
    position = [0.0, 3.2, 3000.0]
    velocity = [1.0, 3.0, 0.0]
    angular_velocity = [0.01, 0.01, 0.0]

    state: State = State(instant, position, velocity, quaternion, angular_velocity, frame)

    ans_state = state.in_frame(Frame.GCRF())

    assert ans_state is not None
    assert isinstance(ans_state, State)

################################################################################################################################################################

def test_flight_profile_state_undefined ():

    state: State = State.undefined()

    assert state is not None
    assert isinstance(state, State)
    assert state.is_defined() is False

################################################################################################################################################################
