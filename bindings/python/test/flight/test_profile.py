# Apache License 2.0

from datetime import datetime

import pytest

from ostk.mathematics.geometry.d3.transformations.rotations import Quaternion

from ostk.physics import Environment
from ostk.physics.time import DateTime
from ostk.physics.time import Time
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.units import Length
from ostk.physics.coordinate import Transform
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Axes
from ostk.physics.coordinate.frame.providers import Dynamic as DynamicProvider

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.flight.profile import State
from ostk.astrodynamics.flight.profile.models import Transform as TransformModel
from ostk.astrodynamics.flight.profile.models import Tabulated as TabulatedModel


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2020, 1, 3), Scale.UTC)


@pytest.fixture
def profile() -> Profile:
    def dynamic_provider_generator(instant: Instant):
        return Transform.identity(instant)

    return Profile(
        model=TransformModel(
            dynamic_provider=DynamicProvider(dynamic_provider_generator),
            frame=Frame.GCRF(),
        ),
    )


class TestProfile:
    def test_constructors(self, profile: Profile):
        assert profile is not None
        assert isinstance(profile, Profile)

    def test_get_state_at(self, profile: Profile, instant: Instant):
        state: State = profile.get_state_at(instant)

        assert state is not None
        assert isinstance(state, State)
        state.is_defined()

    def test_get_states_at(self, profile: Profile, instant: Instant):
        states = profile.get_states_at([instant, instant])

        assert states is not None

    def test_get_axes_at(self, profile: Profile, instant: Instant):
        axes = profile.get_axes_at(instant)

        assert axes is not None
        assert isinstance(axes, Axes)

    def test_get_body_frame(self, profile: Profile, instant: Instant):
        frame = profile.get_body_frame("Name")

        assert frame is not None
        assert isinstance(frame, Frame)

    def test_undefined(self):
        profile: Profile = Profile.undefined()

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined() is False

    def test_inertial_pointing(self):
        quaternion: Quaternion = Quaternion([0.0, 0.0, 0.0, 1.0], Quaternion.Format.XYZS)

        trajectory: Trajectory = Trajectory.position(
            Position.meters((0.0, 0.0, 0.0), Frame.GCRF())
        )

        profile: Profile = Profile.inertial_pointing(trajectory, quaternion)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_nadir_pointing(self):
        environment = Environment.default()

        orbit = Orbit.sun_synchronous(
            epoch=Instant.date_time(datetime(2020, 1, 1, 0, 0, 0), Scale.UTC),
            altitude=Length.kilometers(500.0),
            local_time_at_descending_node=Time(14, 0, 0),
            celestial_object=environment.access_celestial_object_with_name("Earth"),
        )

        profile: Profile = Profile.nadir_pointing(orbit, Orbit.FrameType.VVLH)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_tabulated(self):
        profile = Profile(
            model=TabulatedModel(
                states=[
                    State(
                        instant=Instant.date_time(
                            datetime(2020, 1, 1, 0, 0, 0), Scale.UTC
                        ),
                        position=Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                        velocity=Velocity.meters_per_second(
                            (0.0, 0.0, 0.0), Frame.GCRF()
                        ),
                        attitude=Quaternion.unit(),
                        angular_velocity=(0.0, 0.0, 0.0),
                        reference_frame=Frame.GCRF(),
                    ),
                    State(
                        instant=Instant.date_time(
                            datetime(2020, 1, 1, 0, 1, 0), Scale.UTC
                        ),
                        position=Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                        velocity=Velocity.meters_per_second(
                            (0.0, 0.0, 0.0), Frame.GCRF()
                        ),
                        attitude=Quaternion.unit(),
                        angular_velocity=(0.0, 0.0, 0.0),
                        reference_frame=Frame.GCRF(),
                    ),
                ],
            ),
        )

        assert isinstance(profile, Profile)
        assert profile.is_defined()

        assert (
            profile.get_state_at(
                Instant.date_time(datetime(2020, 1, 1, 0, 0, 30), Scale.UTC)
            )
            is not None
        )
