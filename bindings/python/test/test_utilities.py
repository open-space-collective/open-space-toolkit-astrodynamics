# Apache License 2.0

import pytest

import ostk.physics as physics
import ostk.astrodynamics as astrodynamics
from ostk.astrodynamics import utilities

Length = physics.units.Length
Angle = physics.units.Angle
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Instant = physics.time.Instant
Interval = physics.time.Interval
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
LLA = physics.coordinate.spherical.LLA
AER = physics.coordinate.spherical.AER
Environment = physics.Environment
Trajectory = astrodynamics.Trajectory
State = astrodynamics.flight.profile.State


class TestUtilities:
    def test_lla_from_state(self, state: State):
        lla: list = utilities.lla_from_state(state)

        assert lla is not None
        assert len(lla) == 3
        assert isinstance(lla, list)
        assert isinstance(lla[0], float)
        assert lla[0] >= -90.0 and lla[0] <= 90.0
        assert isinstance(lla[1], float)
        assert lla[0] >= -180.0 and lla[0] <= 180.0
        assert isinstance(lla[2], float)

    def test_lla_from_position(
        self,
        instant: Instant,
        position: Position,
        state: State,
    ):
        lla: LLA = utilities.lla_from_position(position, instant)

        assert lla is not None
        assert isinstance(lla, LLA)
        assert lla.is_defined()

        lla_from_state: list = utilities.lla_from_state(state)

        assert lla.get_latitude().in_degrees() == lla_from_state[0]
        assert lla.get_longitude().in_degrees() == lla_from_state[1]
        assert lla.get_altitude().in_meters() == lla_from_state[2]

    def test_position_from_lla(self, lla: LLA):
        position: Position = utilities.position_from_lla(lla)

        assert position is not None
        assert isinstance(position, Position)
        assert position.is_defined()

    def test_compute_aer(
        self, instant: Instant, position: Position, environment: Environment
    ):
        aer: list = utilities.compute_aer(instant, position, position, environment)

        assert aer is not None
        assert len(aer) == 3
        assert aer[2] == 0.0

    def test_compute_time_lla_aer_state(
        self, state: State, position: Position, environment: Environment
    ):
        time_lla_aer: list = utilities.compute_time_lla_aer_state(
            state, position, environment
        )

        assert time_lla_aer is not None
        assert len(time_lla_aer) == 7
        assert isinstance(time_lla_aer[0], Instant)
        assert time_lla_aer[0] == state.get_instant()
        for i in range(1, 7):
            assert isinstance(time_lla_aer[i], float)

    def test_compute_trajectory_geometry(
        self, interval: Interval, trajectory: Trajectory
    ):
        output: list = utilities.compute_trajectory_geometry(trajectory, interval)

        assert output is not None
        assert len(output) == 2

    def test_convert_state(self, instant: Instant, state: State):
        output: list = utilities.convert_state(instant, state)

        assert output is not None
        assert len(output) == 11
        assert isinstance(output[0], str)
        for i in range(1, 11):
            assert isinstance(output[i], float)
