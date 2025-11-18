# Apache License 2.0

from datetime import datetime
from datetime import timezone

import pytest

import pandas as pd

from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER

from ostk.astrodynamics import utilities
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.converters import coerce_to_datetime
from ostk.astrodynamics.dataframe import generate_orbit_from_dataframe
from ostk.astrodynamics.dataframe import generate_states_from_dataframe

@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime.parse("2024-01-29T00:00:00"), Scale.UTC)

@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()

@pytest.fixture
def position_1(frame: Frame) -> Position:
    return Position.meters([7000000.0, 0.0, 0.0], frame)

@pytest.fixture
def velocity_1(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)

@pytest.fixture
def position_2(frame: Frame) -> Position:
    return Position.meters([7000100.0, 0.0, 0.0], frame)

@pytest.fixture
def velocity_2(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)

@pytest.fixture
def state_1(
    instant: Instant,
    position_1: Position,
    velocity_1: Velocity,
) -> State:
    return State(instant=instant, position=position_1, velocity=velocity_1)

@pytest.fixture
def state_2(
    instant: Instant,
    position_2: Position,
    velocity_2: Velocity,
) -> State:
    return State(instant=instant, position=position_2, velocity=velocity_2)

@pytest.fixture
def states_1(state_1: State) -> list[State]:
    return [state_1, state_1, state_1]

@pytest.fixture
def states_2(state_2: State) -> list[State]:
    return [state_2, state_2, state_2]

@pytest.fixture
def orbit_dataframe(instant: Instant) -> pd.DataFrame:
    return pd.DataFrame(
        [
            {
                "Timestamp": coerce_to_datetime(instant),
                "r_GCRF_x": 7000000.0,
                "r_GCRF_y": 0.0,
                "r_GCRF_z": 0.0,
                "v_GCRF_x": 0.0,
                "v_GCRF_y": 7500.0,
                "v_GCRF_z": 0.0,
            },
            {
                "Timestamp": coerce_to_datetime(instant + Duration.seconds(60.0)),
                "r_GCRF_x": 7000100.0,
                "r_GCRF_y": 450000.0,
                "r_GCRF_z": 0.0,
                "v_GCRF_x": 0.0,
                "v_GCRF_y": 7500.0,
                "v_GCRF_z": 0.0,
            },
        ]
    )


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
        time_lla_aer: tuple[datetime, float, float, float, float, float, float] = (
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


    def test_compare_states_to_states_identical_states(
        self,
        states_1: list[State],
    ) -> None:
        residuals = utilities.compare_states_to_states(states_1, states_1)

        assert len(residuals) == 3
        for residual in residuals:
            assert isinstance(residual, dict)
            assert residual["dr"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dr_x"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dr_y"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dr_z"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv_x"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv_y"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv_z"] == pytest.approx(0.0, abs=1e-10)

    def test_compare_states_to_states_different_states(
        self,
        states_1: list[State],
        states_2: list[State],
    ) -> None:
        residuals = utilities.compare_states_to_states(states_1, states_2)

        assert len(residuals) == 3
        # Expected difference: states_1 - states_2 = 7000000 - 7000100 = -100m in x-direction
        expected_residual_magnitude = 100.0
        expected_residual_x = -100.0
        for residual in residuals:
            assert isinstance(residual, dict)
            assert residual["dr"] == pytest.approx(expected_residual_magnitude, rel=1e-6)
            assert residual["dr_x"] == pytest.approx(expected_residual_x, rel=1e-6)
            assert residual["dr_y"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dr_z"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv"] == pytest.approx(0.0, abs=1e-10)

    def test_compare_states_to_states_returns_list_of_dicts(
        self,
        states_1: list[State],
        states_2: list[State],
    ) -> None:
        residuals = utilities.compare_states_to_states(states_1, states_2)

        assert isinstance(residuals, list)
        for residual in residuals:
            assert isinstance(residual, dict)
            assert "timestamp" in residual
            assert "dr" in residual
            assert "dr_x" in residual
            assert "dr_y" in residual
            assert "dr_z" in residual
            assert "dv" in residual
            assert "dv_x" in residual
            assert "dv_y" in residual
            assert "dv_z" in residual
            assert isinstance(residual["timestamp"], datetime)
            assert isinstance(residual["dr"], float)
            assert isinstance(residual["dv"], float)

    def test_compare_orbit_to_states_returns_list_of_dicts(
        self,
        orbit_dataframe: pd.DataFrame,
    ) -> None:
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe, interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = utilities.compare_orbit_to_states(orbit, states=states)

        assert isinstance(result, list)
        assert len(result) == len(states)
        for entry in result:
            assert isinstance(entry, dict)
            assert "timestamp" in entry
            assert "dr" in entry
            assert "dr_x" in entry
            assert "dr_y" in entry
            assert "dr_z" in entry
            assert "dv" in entry
            assert "dv_x" in entry
            assert "dv_y" in entry
            assert "dv_z" in entry
            assert isinstance(entry["timestamp"], datetime)
            assert isinstance(entry["dr"], float)
            assert isinstance(entry["dv"], float)

    def test_compare_orbit_to_states_identical_orbit_and_states(
        self,
        orbit_dataframe: pd.DataFrame,
    ) -> None:
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe,
            interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = utilities.compare_orbit_to_states(orbit, states=states)

        assert len(result) == 2
        for entry in result:
            # Should be very small due to numerical precision and interpolation
            assert entry["dr"] < 1.0  # Less than 1 meter
            assert entry["dv"] < 0.01  # Less than 1 cm/s

    def test_compare_orbit_to_states_datetime_ordering(
        self,
        orbit_dataframe: pd.DataFrame,
        instant: Instant,
    ) -> None:
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe,
            interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = utilities.compare_orbit_to_states(orbit, states=states)

        expected_datetimes = [
            instant.get_date_time(Scale.UTC).replace(tzinfo=timezone.utc),
            (instant + Duration.seconds(60.0)).get_date_time(Scale.UTC).replace(tzinfo=timezone.utc),
        ]

        for i, entry in enumerate(result):
            assert entry["timestamp"] == expected_datetimes[i]
