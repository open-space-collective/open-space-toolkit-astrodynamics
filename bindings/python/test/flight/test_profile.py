################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_profile.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

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
State = astrodynamics.flight.profile.State

################################################################################################################################################################

@pytest.fixture
def instant () -> Instant:

    return Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

@pytest.fixture
def profile () -> Profile:

    def dynamic_provider_generator (instant: Instant):
        return Transform.identity(instant)

    return Profile(DynamicProvider(dynamic_provider_generator), Frame.GCRF())

################################################################################################################################################################

class TestProfile:

    def test_constructors (self, profile: Profile):

        assert profile is not None
        assert isinstance(profile, Profile)

    def test_get_state_at (self, profile: Profile, instant: Instant):

        state: State = profile.get_state_at(instant)

        assert state is not None
        assert isinstance(state, State)
        state.is_defined()

    def test_get_states_at (self, profile: Profile, instant: Instant):

        states = profile.get_states_at([instant, instant])

        assert states is not None

    def test_get_axes_at (self, profile: Profile, instant: Instant):

        axes = profile.get_axes_at(instant)

        assert axes is not None
        assert isinstance(axes, Axes)

    def test_undefined (self):

        profile: Profile = Profile.undefined()

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined() is False

    # def test_inertial_pointing (self):

    #     quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    #     trajectory: Trajectory = Trajectory.

    #     profile: Profile = Profile.inertial_pointing(quaternion, trajectory)

    #     assert profile is not None
    #     assert isinstance(profile, Profile)
    #     assert profile.is_defined()

    # def test_nadir_pointing (self):

    #     quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

    #     trajectory: Trajectory = Trajectory.

    #     profile: Profile = Profile.inertial_pointing(quaternion, trajectory)

    #     assert profile is not None
    #     assert isinstance(profile, Profile)
    #     assert profile.is_defined()

################################################################################################################################################################
