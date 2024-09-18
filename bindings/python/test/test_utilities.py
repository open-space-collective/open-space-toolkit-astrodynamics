# Apache License 2.0

from datetime import datetime

from ostk.physics import Environment
from ostk.physics.time import Instant, Interval
from ostk.physics.coordinate import Position
from ostk.physics.coordinate.spherical import LLA, AER

from ostk.astrodynamics import utilities
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State


class TestUtility:
    def test_lla_from_state(
        self,
        state: State,
    ):
        lla: LLA = utilities.lla_from_state(state)

        assert lla is not None
        assert isinstance(lla, LLA)
        assert lla.is_defined()

    def test_lla_from_position(
        self,
        instant: Instant,
        position: Position,
    ):
        lla: LLA = utilities.lla_from_position(position, instant)

        assert lla is not None
        assert isinstance(lla, LLA)
        assert lla.is_defined()

    def test_position_from_lla(
        self,
        lla: LLA,
    ):
        position: Position = utilities.position_from_lla(lla)

        assert position is not None
        assert isinstance(position, Position)
        assert position.is_defined()

    def test_compute_aer(
        self,
        instant: Instant,
        position: Position,
        environment: Environment,
    ):
        aer: AER = utilities.compute_aer(instant, position, position, environment)

        assert aer is not None
        assert isinstance(aer, AER)

    def test_compute_time_lla_aer_coordinates(
        self,
        state: State,
        position: Position,
        environment: Environment,
    ):
        time_lla_aer: float[datetime, float, float, float, float, float, float] = (
            utilities.compute_time_lla_aer_coordinates(state, position, environment)
        )

        assert time_lla_aer is not None
        assert len(time_lla_aer) == 7
        assert isinstance(time_lla_aer[0], datetime)
        for i in range(1, 7):
            assert isinstance(time_lla_aer[i], float)

    def test_compute_trajectory_geometry(
        self,
        interval: Interval,
        trajectory: Trajectory,
    ):
        output: list[LLA] = utilities.compute_trajectory_geometry(trajectory, interval)

        assert output is not None
        assert len(output) == 2
        assert isinstance(output[0], LLA)

    def test_compute_ground_track(
        self,
        interval: Interval,
        trajectory: Trajectory,
    ):
        output: list[LLA] = utilities.compute_ground_track(trajectory, interval)

        assert output is not None
        assert len(output) == 2
        assert isinstance(output[0], LLA)
        assert output[0].get_altitude().in_meters() <= 15.0

    def test_convert_state(self, state: State):
        output: tuple[
            str, float, float, float, float, float, float, float, float, float
        ] = utilities.convert_state(state)

        assert output is not None
        assert len(output) == 11
        assert isinstance(output[0], str)
        for i in range(1, 11):
            assert isinstance(output[i], float)
