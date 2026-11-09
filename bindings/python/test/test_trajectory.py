# Apache License 2.0

import pytest
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit, State
from ostk.physics.coordinate import Frame, Position
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Duration, Instant
from ostk.physics.unit import Angle, Derived, Length


@pytest.fixture
def start_lla() -> LLA:
    return LLA.vector([0.0, 0.0, 0.0])


@pytest.fixture
def end_lla() -> LLA:
    return LLA.vector([1.0, 0.0, 0.0])


@pytest.fixture
def start_instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def ground_speed() -> Derived:
    return Derived(7000.0, Derived.Unit.meter_per_second())


@pytest.fixture
def earth() -> Earth:
    return Earth.WGS84()


@pytest.fixture
def instants() -> list[Instant]:
    return [Instant.J2000(), Instant.J2000() + Duration.seconds(10.0)]


@pytest.fixture
def position() -> Position:
    return Position.meters([0.0, 0.0, 0.0], Frame.ITRF())


@pytest.fixture
def trajectory(position: Position) -> Trajectory:
    return Trajectory.position(position)


@pytest.fixture
def states(trajectory: Trajectory) -> list[State]:
    # The default (barycentric rational) interpolation requires at least 4 states.
    return trajectory.get_states_at(
        [Instant.J2000() + Duration.seconds(10.0 * i) for i in range(4)]
    )


@pytest.fixture
def orbit() -> Orbit:
    return Orbit.circular(
        epoch=Instant.J2000(),
        altitude=Length.meters(545000.0),
        inclination=Angle.degrees(0.0),
        celestial_object=Earth.WGS84(),
    )


class TestTrajectory:
    def test_trajectory(
        self,
        states: list[State],
    ):
        assert Trajectory(states) is not None

    def test_get_model(
        self,
        trajectory: Trajectory,
    ):
        assert trajectory.get_model() is not None

    def test_get_state_at(
        self,
        trajectory: Trajectory,
    ):
        assert trajectory.get_state_at(Instant.J2000()) is not None

    def test_get_states_at(
        self,
        trajectory: Trajectory,
    ):
        assert (
            trajectory.get_states_at(
                [Instant.J2000(), Instant.J2000() + Duration.seconds(10.0)]
            )
            is not None
        )

    def test_trajectory_without_enough_states_failure(self):
        assert not hasattr(Trajectory, "undefined")
        assert not hasattr(Trajectory, "is_defined")

        with pytest.raises(Exception):
            Trajectory([])

    def test_trajectory_position(
        self,
        position: Position,
    ):
        trajectory: Trajectory = Trajectory.position(position)

        assert trajectory is not None
