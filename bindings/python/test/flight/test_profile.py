################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_profile.py
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
Scale = physics.time.Scale
Instant = physics.time.Instant
Transform = physics.coordinate.Transform
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.providers.Dynamic
Trajectory = astrodynamics.Trajectory
Profile = astrodynamics.flight.Profile
State =  astrodynamics.flight.profile.State

################################################################################################################################################################

def construct_profile ():

    # Construct arbitrary DynamicFrameProvider
    def dynamic_provider_generator (instant: Instant):

        return Transform.identity(instant)

    dynamic_provider = DynamicProvider(dynamic_provider_generator)

    # Construct arbitrary frame
    frame: Frame = Frame.GCRF()

    return Profile(dynamic_provider, frame)

################################################################################################################################################################

def test_flight_profile_constructors ():

    # Construct Profile
    profile: Profile = construct_profile()

    assert profile is not None
    assert isinstance(profile, Profile)

################################################################################################################################################################

# def test_flight_profile_comparators ():

################################################################################################################################################################

def test_flight_profile_undefined ():

    profile: Profile = Profile.undefined()

    assert profile is not None
    assert isinstance(profile, Profile)
    assert profile.is_defined() is False

################################################################################################################################################################

# def test_flight_profile_inertial_pointing ():

#     quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

#     trajectory: Trajectory = Trajectory.

#     profile: Profile = Profile.inertial_pointing(quaternion, trajectory)

#     assert profile is not None
#     assert isinstance(profile, Profile)
#     assert profile.is_defined()

################################################################################################################################################################

# def test_flight_profile_nadir_pointing ():

#     quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

#     trajectory: Trajectory = Trajectory.

#     profile: Profile = Profile.inertial_pointing(quaternion, trajectory)

#     assert profile is not None
#     assert isinstance(profile, Profile)
#     assert profile.is_defined()

################################################################################################################################################################

def test_flight_profile_getters ():

    profile: Profile = construct_profile()

    # get_state_at
    instant: Instant = Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

    state: State = profile.get_state_at(instant)

    assert state is not None
    assert isinstance(state, State)
    state.is_defined()

    # get_states_at
    states = profile.get_states_at([instant, instant])

    assert states is not None

    # get_axes_at
    axes = profile.get_axes_at(instant)

    assert axes is not None
    assert isinstance(axes, Axes)

################################################################################################################################################################
