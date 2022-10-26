################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_profile.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

from datetime import datetime

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Quaternion = mathematics.geometry.d3.transformations.rotations.Quaternion
Environment = physics.Environment
DateTime = physics.time.DateTime
Time = physics.time.Time
Scale = physics.time.Scale
Instant = physics.time.Instant
Length = physics.units.Length
Transform = physics.coordinate.Transform
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.providers.Dynamic
Trajectory = astrodynamics.Trajectory
Orbit = astrodynamics.trajectory.Orbit
Profile = astrodynamics.flight.Profile
State = astrodynamics.flight.profile.State
TransformModel = astrodynamics.flight.profile.models.Transform
TabulatedModel = astrodynamics.flight.profile.models.Tabulated

################################################################################################################################################################

@pytest.fixture
def instant () -> Instant:

    return Instant.date_time(DateTime(2020, 1, 3, 0, 0, 0), Scale.UTC)

@pytest.fixture
def profile () -> Profile:

    def dynamic_provider_generator (instant: Instant):
        return Transform.identity(instant)

    return Profile(TransformModel(DynamicProvider(dynamic_provider_generator), Frame.GCRF()))

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

    def test_get_body_frame (self, profile: Profile, instant: Instant):

        frame = profile.get_body_frame("Name")

        assert frame is not None
        assert isinstance(frame, Frame)

    def test_undefined (self):

        profile: Profile = Profile.undefined()

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined() is False

    def test_inertial_pointing (self):

        quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

        trajectory: Trajectory = Trajectory.position(Position.meters((0.0, 0.0, 0.0), Frame.GCRF()))

        profile: Profile = Profile.inertial_pointing(trajectory, quaternion)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_nadir_pointing (self):

        environment = Environment.default()

        orbit = Orbit.sun_synchronous(
            epoch = Instant.date_time(datetime(2020, 1, 1, 0, 0, 0), Scale.UTC),
            altitude = Length.kilometers(500.0),
            local_time_at_descending_node = Time(14, 0, 0),
            celestial_object = environment.access_celestial_object_with_name('Earth'),
        )

        profile: Profile = Profile.nadir_pointing(orbit, Orbit.FrameType.VVLH)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_tabulated (self):

        tabulated_model = TabulatedModel(
            states = [
                State(
                    instant = Instant.date_time(datetime(2020, 1, 1, 0, 0, 0), Scale.UTC),
                    position = Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                    velocity = Velocity.meters_per_second((0.0, 0.0, 0.0), Frame.GCRF()),
                    attitude = Quaternion.unit(),
                    angular_velocity = (0.0, 0.0, 0.0),
                    reference_frame = Frame.GCRF(),
                ),
                State(
                    instant = Instant.date_time(datetime(2020, 1, 1, 0, 1, 0), Scale.UTC),
                    position = Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                    velocity = Velocity.meters_per_second((0.0, 0.0, 0.0), Frame.GCRF()),
                    attitude = Quaternion.unit(),
                    angular_velocity = (0.0, 0.0, 0.0),
                    reference_frame = Frame.GCRF(),
                ),
            ],
        )

        profile = Profile(
            model = tabulated_model,
        )

        assert isinstance(profile, Profile)
        assert profile.is_defined()

        assert profile.get_state_at(Instant.date_time(datetime(2020, 1, 1, 0, 0, 30), Scale.UTC)) is not None

################################################################################################################################################################
