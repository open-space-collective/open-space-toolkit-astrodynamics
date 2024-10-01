# Apache License 2.0

import pytest

from ostk.physics.environment.object.celestial import Earth
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.unit import Derived

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State


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
    return trajectory.get_states_at(
        [Instant.J2000(), Instant.J2000() + Duration.seconds(10.0)]
    )


class TestTrajectory:
    def test_trajectory(self, states: list[State]):
        assert Trajectory(states) is not None

    def test_is_defined(self, trajectory: Trajectory):
        assert trajectory.is_defined()

    def test_access_model(self, trajectory: Trajectory):
        assert trajectory.access_model() is not None

    def test_get_state_at(self, trajectory: Trajectory):
        assert trajectory.get_state_at(Instant.J2000()) is not None

    def test_get_states_at(self, trajectory: Trajectory):
        assert (
            trajectory.get_states_at(
                [Instant.J2000(), Instant.J2000() + Duration.seconds(10.0)]
            )
            is not None
        )

    def test_trajectory_undefined(self):
        trajectory: Trajectory = Trajectory.undefined()

        assert trajectory is not None
        assert isinstance(trajectory, Trajectory)
        assert trajectory.is_defined() is False

    def test_trajectory_position(self, position: Position):
        trajectory: Trajectory = Trajectory.position(position)

        assert trajectory is not None

    def test_ground_strip(
        self,
        start_lla: LLA,
        end_lla: LLA,
        ground_speed: Derived,
        start_instant: Instant,
        earth: Earth,
        instants: list[Instant],
    ):
        assert (
            Trajectory.ground_strip(
                start_lla, end_lla, ground_speed, start_instant, earth
            )
            is not None
        )
        assert (
            Trajectory.ground_strip(start_lla, end_lla, ground_speed, start_instant)
            is not None
        )

        assert Trajectory.ground_strip(start_lla, end_lla, instants, earth) is not None
        assert Trajectory.ground_strip(start_lla, end_lla, instants) is not None
