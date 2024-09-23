# Apache License 2.0

from datetime import datetime

import pytest

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics import Environment
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Time
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.unit import Length
from ostk.physics.coordinate import Transform
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Axes
from ostk.physics.coordinate.frame.provider import Dynamic as DynamicProvider

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.flight.profile.model import Transform as TransformModel
from ostk.astrodynamics.flight.profile.model import Tabulated as TabulatedModel


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2020, 1, 1, 0, 0, 30), Scale.UTC)


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def orbit(instant: Instant, environment: Environment) -> Orbit:
    return Orbit.sun_synchronous(
        epoch=instant,
        altitude=Length.kilometers(500.0),
        local_time_at_descending_node=Time(14, 0, 0),
        celestial_object=environment.access_celestial_object_with_name("Earth"),
    )


@pytest.fixture
def transform_model() -> TransformModel:
    def dynamic_provider_generator(instant: Instant):
        return Transform.identity(instant)

    return TransformModel(
        dynamic_provider=DynamicProvider(dynamic_provider_generator),
        frame=Frame.GCRF(),
    )


@pytest.fixture
def tabulated_model() -> TabulatedModel:
    return TabulatedModel(
        states=[
            State(
                instant=Instant.date_time(datetime(2020, 1, 1, 0, 0, 0), Scale.UTC),
                position=Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                velocity=Velocity.meters_per_second((0.0, 0.0, 0.0), Frame.GCRF()),
                attitude=Quaternion.unit(),
                angular_velocity=(0.0, 0.0, 0.0),
                attitude_frame=Frame.GCRF(),
            ),
            State(
                instant=Instant.date_time(datetime(2020, 1, 1, 0, 1, 0), Scale.UTC),
                position=Position.meters((0.0, 0.0, 0.0), Frame.GCRF()),
                velocity=Velocity.meters_per_second((0.0, 0.0, 0.0), Frame.GCRF()),
                attitude=Quaternion.unit(),
                angular_velocity=(0.0, 0.0, 0.0),
                attitude_frame=Frame.GCRF(),
            ),
        ],
    )


@pytest.fixture(
    params=[
        "transform_model",
        "tabulated_model",
    ]
)
def profile(request) -> Profile:
    model: TransformModel | TabulatedModel = request.getfixturevalue(request.param)
    return Profile(model=model)


@pytest.fixture(
    params=[
        Profile.Target(Profile.TargetType.GeocentricNadir, Profile.Axis.X),
        Profile.TrajectoryTarget(
            Trajectory.position(Position.meters((0.0, 0.0, 0.0), Frame.ITRF())),
            Profile.Axis.X,
        ),
        Profile.OrientationProfileTarget(
            [
                (Instant.J2000(), [1.0, 0.0, 0.0]),
                (Instant.J2000() + Duration.minutes(1.0), [1.0, 0.0, 0.0]),
                (Instant.J2000() + Duration.minutes(2.0), [1.0, 0.0, 0.0]),
                (Instant.J2000() + Duration.minutes(3.0), [1.0, 0.0, 0.0]),
            ],
            Profile.Axis.X,
        ),
        Profile.CustomTarget(
            lambda state: [1.0, 0.0, 0.0],
            Profile.Axis.X,
        ),
    ]
)
def alignment_target() -> Profile.Target:
    return Profile.Target(Profile.TargetType.GeocentricNadir, Profile.Axis.X)


@pytest.fixture
def clocking_target() -> Profile.Target:
    return Profile.Target(Profile.TargetType.VelocityECI, Profile.Axis.Y)


class TestProfile:
    def test_constructors(self, profile: Profile):
        assert profile is not None
        assert isinstance(profile, Profile)

    def test_profile_target(self, alignment_target: Profile.Target):
        assert alignment_target is not None
        assert isinstance(alignment_target, Profile.Target)

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

    def test_get_body_frame(self, profile: Profile):
        if Frame.exists("Name"):
            Frame.destruct("Name")

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
            Position.meters((0.0, 0.0, 0.0), Frame.ITRF())
        )

        profile: Profile = Profile.inertial_pointing(trajectory, quaternion)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_nadir_pointing(
        self,
        orbit: Orbit,
    ):
        profile: Profile = Profile.nadir_pointing(orbit, Orbit.FrameType.VVLH)

        assert profile is not None
        assert isinstance(profile, Profile)
        assert profile.is_defined()

    def test_custom_pointing(
        self,
        orbit: Orbit,
        alignment_target: Profile.Target,
        clocking_target: Profile.Target,
    ):
        profile = Profile.custom_pointing(
            orbit, Profile.align_and_constrain(alignment_target, clocking_target)
        )

        assert profile is not None
        assert profile.is_defined()

    def test_align_and_constrain(
        self,
        orbit: Orbit,
        instant: Instant,
        alignment_target: Profile.Target,
        clocking_target: Profile.Target,
    ):
        orientation = Profile.align_and_constrain(
            alignment_target=alignment_target,
            clocking_target=clocking_target,
        )

        assert orientation is not None
        assert orientation(orbit.get_state_at(instant)) is not None

    def test_custom_pointing(
        self,
        orbit: Orbit,
        alignment_target: Profile.Target,
        clocking_target: Profile.Target,
    ):
        profile = Profile.custom_pointing(
            orbit, Profile.align_and_constrain(alignment_target, clocking_target)
        )

        assert profile is not None
        assert profile.is_defined()

        profile = Profile.custom_pointing(
            orbit,
            alignment_target,
            clocking_target,
        )

        assert profile is not None
        assert profile.is_defined()
