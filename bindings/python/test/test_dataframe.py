# Copyright © Loft Orbital Solutions Inc.

import pytest

import numpy as np

import pandas as pd

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate.frame.provider.iau import Theory

from ostk.astrodynamics.converters import coerce_to_datetime
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.flight import Profile

from ostk.astrodynamics.dataframe import generate_states_from_dataframe
from ostk.astrodynamics.dataframe import generate_dataframe_from_states
from ostk.astrodynamics.dataframe import generate_profile_from_dataframe
from ostk.astrodynamics.dataframe import generate_dataframe_from_profile


class TestOrbitDataframe:
    @pytest.fixture
    def instant(self) -> Instant:
        return Instant.date_time(DateTime.parse("2024-01-29T00:00:00"), Scale.UTC)

    @pytest.fixture
    def frame(self) -> Frame:
        return Frame.GCRF()

    @pytest.fixture
    def position(self, frame: Frame) -> Position:
        return Position.meters(
            [755972.142139276024, -3390511.949699319433, 5955672.751532567665],
            frame,
        )

    @pytest.fixture
    def velocity(self, frame: Frame) -> Velocity:
        return Velocity.meters_per_second(
            [-563.764594800880, -6619.592151780337, -3685.668514834143],
            frame,
        )

    @pytest.fixture
    def attitude(self) -> Quaternion:
        return Quaternion.xyzs(
            -0.638160707740, -0.163520830523, 0.726693549038, 0.194751982966
        )

    @pytest.fixture
    def angular_velocity(self) -> np.ndarray:
        return np.array([0.0, 0.0, 0.0])

    @pytest.fixture
    def orbit_state(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
    ) -> State:
        return State(
            instant=instant,
            position=position,
            velocity=velocity,
        )

    @pytest.fixture
    def orbit_states(self, orbit_state: State) -> list[State]:
        return [orbit_state, orbit_state, orbit_state]

    @pytest.fixture
    def profile_state(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
        attitude: Quaternion,
        angular_velocity: np.ndarray,
        frame: Frame,
    ) -> State:
        return State(instant, position, velocity, attitude, angular_velocity, frame)

    @pytest.fixture
    def profile_states(self, profile_state: State) -> list[State]:
        return [profile_state, profile_state, profile_state]

    @pytest.fixture
    def profile_dataframe_position_columns(self) -> list[str]:
        return ["r_J2000 (IAU 2006)_x", "r_J2000 (IAU 2006)_y", "r_J2000 (IAU 2006)_z"]

    @pytest.fixture
    def profile_dataframe_velocity_columns(self) -> list[str]:
        return ["v_J2000 (IAU 2006)_x", "v_J2000 (IAU 2006)_y", "v_J2000 (IAU 2006)_z"]

    @pytest.fixture
    def profile_dataframe_attitude_columns(self) -> list[str]:
        return [
            "q_B_J2000 (IAU 2006)_x",
            "q_B_J2000 (IAU 2006)_y",
            "q_B_J2000 (IAU 2006)_z",
            "q_B_J2000 (IAU 2006)_s",
        ]

    @pytest.fixture
    def profile_dataframe_angular_velocity_columns(self) -> list[str]:
        return [
            "w_B_J2000 (IAU 2006)_in_B_x",
            "w_B_J2000 (IAU 2006)_in_B_y",
            "w_B_J2000 (IAU 2006)_in_B_z",
        ]

    @pytest.fixture
    def profile_dataframe(
        self,
        instant: Instant,
        profile_dataframe_position_columns: list[str],
        profile_dataframe_velocity_columns: list[str],
        profile_dataframe_attitude_columns: list[str],
        profile_dataframe_angular_velocity_columns: list[str],
    ) -> pd.DataFrame:
        return pd.DataFrame(
            [
                {
                    "Timestamp": coerce_to_datetime(instant),
                    **dict(zip(profile_dataframe_position_columns, [1.0, 2.0, 3.0])),
                    **dict(zip(profile_dataframe_velocity_columns, [4.0, 5.0, 6.0])),
                    **dict(zip(profile_dataframe_attitude_columns, [0.0, 0.0, 0.0, 1.0])),
                    **dict(
                        zip(profile_dataframe_angular_velocity_columns, [0.0, 0.0, 0.0])
                    ),
                },
                {
                    "Timestamp": coerce_to_datetime(instant + Duration.minutes(1.0)),
                    **dict(zip(profile_dataframe_position_columns, [11.0, 12.0, 13.0])),
                    **dict(zip(profile_dataframe_velocity_columns, [14.0, 15.0, 16.0])),
                    **dict(zip(profile_dataframe_attitude_columns, [0.0, 0.0, 1.0, 0.0])),
                    **dict(
                        zip(profile_dataframe_angular_velocity_columns, [1.0, 1.0, 1.0])
                    ),
                },
            ]
        )

    @pytest.fixture
    def profile_dataframe_indexed_timestamp(
        self,
        profile_dataframe: pd.DataFrame,
    ) -> pd.DataFrame:
        profile_dataframe.set_index("Timestamp", inplace=True)
        return profile_dataframe

    @pytest.fixture
    def orbit_dataframe(self, instant: Instant) -> pd.DataFrame:
        return pd.DataFrame(
            [
                {
                    "Timestamp": coerce_to_datetime(instant),
                    "r_GCRF_x": 1.0,
                    "r_GCRF_y": 2.0,
                    "r_GCRF_z": 3.0,
                    "v_GCRF_x": 4.0,
                    "v_GCRF_y": 5.0,
                    "v_GCRF_z": 6.0,
                },
                {
                    "Timestamp": coerce_to_datetime(instant + Duration.minutes(1.0)),
                    "r_GCRF_x": 11.0,
                    "r_GCRF_y": 12.0,
                    "r_GCRF_z": 13.0,
                    "v_GCRF_x": 14.0,
                    "v_GCRF_y": 15.0,
                    "v_GCRF_z": 16.0,
                },
            ]
        )

    @pytest.fixture
    def orbit_dataframe_indexed_timestamp(
        self, orbit_dataframe: pd.DataFrame
    ) -> pd.DataFrame:
        orbit_dataframe.set_index("Timestamp", inplace=True)
        return orbit_dataframe

    def test_generate_orbit_states_from_dataframe_defaults_success(
        self,
        orbit_dataframe: pd.DataFrame,
    ):
        states: list[State] = generate_states_from_dataframe(orbit_dataframe)

        for state in states:
            assert len(state.get_coordinates()) == len(orbit_dataframe.columns) - 1

    def test_generate_profile_states_from_dataframe_defaults_success(
        self,
        profile_dataframe: pd.DataFrame,
    ):
        states: list[State] = generate_states_from_dataframe(
            profile_dataframe,
            reference_frame=Frame.J2000(Theory.IAU_2006),
        )

        for state in states:
            assert len(state.get_coordinates()) == len(profile_dataframe.columns) - 1

    def test_generate_states_from_profile_dataframe_success(
        self,
        profile_dataframe: pd.DataFrame,
        profile_dataframe_position_columns: list[str],
        profile_dataframe_velocity_columns: list[str],
        profile_dataframe_attitude_columns: list[str],
        profile_dataframe_angular_velocity_columns: list[str],
    ):
        states: list[State] = generate_states_from_dataframe(
            dataframe=profile_dataframe,
            reference_frame=Frame.J2000(Theory.IAU_2006),
            time_column="Timestamp",
            position_columns=profile_dataframe_position_columns,
            velocity_columns=profile_dataframe_velocity_columns,
            attitude_columns=profile_dataframe_attitude_columns,
            angular_velocity_columns=profile_dataframe_angular_velocity_columns,
        )

        for state in states:
            assert len(state.get_coordinates()) == len(profile_dataframe.columns) - 1

    def test_generate_states_from_orbit_dataframe_success(
        self,
        orbit_dataframe: pd.DataFrame,
    ):
        states: list[State] = generate_states_from_dataframe(
            dataframe=orbit_dataframe,
            reference_frame=Frame.GCRF(),
            time_column="Timestamp",
            position_columns=["r_GCRF_x", "r_GCRF_y", "r_GCRF_z"],
            velocity_columns=["v_GCRF_x", "v_GCRF_y", "v_GCRF_z"],
        )

        for state in states:
            assert len(state.get_coordinates()) == len(orbit_dataframe.columns) - 1

    def test_generate_states_from_profile_dataframe_success_defined_columns_without_time(
        self,
        profile_dataframe_indexed_timestamp: pd.DataFrame,
        profile_dataframe_position_columns: list[str],
        profile_dataframe_velocity_columns: list[str],
        profile_dataframe_attitude_columns: list[str],
        profile_dataframe_angular_velocity_columns: list[str],
    ):
        states: list[State] = generate_states_from_dataframe(
            dataframe=profile_dataframe_indexed_timestamp,
            reference_frame=Frame.J2000(Theory.IAU_2006),
            position_columns=profile_dataframe_position_columns,
            velocity_columns=profile_dataframe_velocity_columns,
            attitude_columns=profile_dataframe_attitude_columns,
            angular_velocity_columns=profile_dataframe_angular_velocity_columns,
        )

        for state in states:
            assert len(state.get_coordinates()) == len(
                profile_dataframe_indexed_timestamp.columns
            )

    def test_generate_states_from_orbit_dataframe_success_defined_columnsout_with_time(
        self,
        orbit_dataframe_indexed_timestamp: pd.DataFrame,
    ):
        states: list[State] = generate_states_from_dataframe(
            dataframe=orbit_dataframe_indexed_timestamp,
            reference_frame=Frame.GCRF(),
            position_columns=["r_GCRF_x", "r_GCRF_y", "r_GCRF_z"],
            velocity_columns=["v_GCRF_x", "v_GCRF_y", "v_GCRF_z"],
        )

        for state in states:
            assert len(state.get_coordinates()) == len(
                orbit_dataframe_indexed_timestamp.columns
            )

    def test_generate_dataframe_from_profile_states_success_custom_columns(
        self,
        profile_states: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=profile_states,
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            attitude_columns=["q_1", "q_2", "q_3", "q_4"],
            angular_velocity_columns=["w_1", "w_2", "w_3"],
        )

        assert list(generated_dataframe.columns) == [
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
            "q_1",
            "q_2",
            "q_3",
            "q_4",
            "w_1",
            "w_2",
            "w_3",
        ]

    def test_generate_dataframe_from_orbit_states_success_custom_columns(
        self,
        orbit_states: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=orbit_states,
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
        )

        assert list(generated_dataframe.columns) == [
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
        ]

    def test_generate_dataframe_from_profile_states_success_custom_reference_frame(
        self,
        profile_states: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=profile_states,
            reference_frame=Frame.ITRF(),
        )

        assert list(generated_dataframe.columns) == [
            "r_ITRF_x",
            "r_ITRF_y",
            "r_ITRF_z",
            "v_ITRF_x",
            "v_ITRF_y",
            "v_ITRF_z",
            "q_B_ITRF_x",
            "q_B_ITRF_y",
            "q_B_ITRF_z",
            "q_B_ITRF_s",
            "w_B_ITRF_in_B_x",
            "w_B_ITRF_in_B_y",
            "w_B_ITRF_in_B_z",
        ]

    def test_generate_dataframe_from_orbit_states_success_set_time_index_disabled(
        self,
        orbit_states: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=orbit_states,
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            set_time_index=False,
        )

        assert list(generated_dataframe.columns) == [
            "t",
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
        ]

    def test_generate_dataframe_from_profile_states_success_set_time_index_disabled(
        self,
        profile_states: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=profile_states,
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            attitude_columns=["q_1", "q_2", "q_3", "q_4"],
            angular_velocity_columns=["w_1", "w_2", "w_3"],
            set_time_index=False,
        )

        assert list(generated_dataframe.columns) == [
            "t",
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
            "q_1",
            "q_2",
            "q_3",
            "q_4",
            "w_1",
            "w_2",
            "w_3",
        ]


class TestProfileDataframe:
    @pytest.fixture
    def environment(self) -> Environment:
        return Environment.default()

    @pytest.fixture
    def epoch(self) -> Instant:
        return Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC)

    @pytest.fixture
    def dataframe(self, epoch: Instant) -> pd.DataFrame:
        return pd.DataFrame(
            [
                {
                    "Timestamp": coerce_to_datetime(epoch),
                    "r_GCRF_x": 1.0,
                    "r_GCRF_y": 2.0,
                    "r_GCRF_z": 3.0,
                    "v_GCRF_x": 4.0,
                    "v_GCRF_y": 5.0,
                    "v_GCRF_z": 6.0,
                    "q_B_GCRF_x": 0.0,
                    "q_B_GCRF_y": 0.0,
                    "q_B_GCRF_z": 0.0,
                    "q_B_GCRF_s": 1.0,
                    "w_B_GCRF_in_B_x": 0.0,
                    "w_B_GCRF_in_B_y": 0.0,
                    "w_B_GCRF_in_B_z": 0.0,
                },
                {
                    "Timestamp": coerce_to_datetime(epoch + Duration.minutes(1.0)),
                    "r_GCRF_x": 11.0,
                    "r_GCRF_y": 12.0,
                    "r_GCRF_z": 13.0,
                    "v_GCRF_x": 14.0,
                    "v_GCRF_y": 15.0,
                    "v_GCRF_z": 16.0,
                    "q_B_GCRF_x": 0.0,
                    "q_B_GCRF_y": 0.0,
                    "q_B_GCRF_z": 1.0,
                    "q_B_GCRF_s": 0.0,
                    "w_B_GCRF_in_B_x": 1.0,
                    "w_B_GCRF_in_B_y": 1.0,
                    "w_B_GCRF_in_B_z": 1.0,
                },
            ]
        )

    @pytest.fixture
    def profile(self, dataframe: pd.DataFrame) -> Profile:
        return generate_profile_from_dataframe(
            dataframe=dataframe,
        )

    @pytest.fixture
    def dataframe_indexed_timestamp(self, dataframe: pd.DataFrame) -> pd.DataFrame:
        dataframe.set_index("Timestamp", inplace=True)
        return dataframe

    def test_generate_profile_from_dataframe_success(
        self,
        epoch: Instant,
        dataframe: pd.DataFrame,
    ):
        profile: Profile = generate_profile_from_dataframe(
            dataframe=dataframe,
        )

        assert profile is not None

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_position().get_coordinates(),
            np.array((1.0, 2.0, 3.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_velocity().get_coordinates(),
            np.array((4.0, 5.0, 6.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_attitude().to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 0.0, 1.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_angular_velocity(),
            np.array((0.0, 0.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_position()
            .get_coordinates(),
            np.array((11.0, 12.0, 13.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_velocity()
            .get_coordinates(),
            np.array((14.0, 15.0, 16.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_attitude()
            .to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 1.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0)).get_angular_velocity(),
            np.array((1.0, 1.0, 1.0)),
            atol=1e-8,
        )

        assert profile.get_state_at(epoch).get_frame() == Frame.GCRF()

    def test_generate_profile_from_dataframe_success_defined_columns_with_time(
        self,
        epoch: Instant,
        dataframe: pd.DataFrame,
    ):
        profile: Profile = generate_profile_from_dataframe(
            dataframe=dataframe,
            time_column="Timestamp",
            position_columns=["r_GCRF_x", "r_GCRF_y", "r_GCRF_z"],
            velocity_columns=["v_GCRF_x", "v_GCRF_y", "v_GCRF_z"],
            attitude_columns=["q_B_GCRF_x", "q_B_GCRF_y", "q_B_GCRF_z", "q_B_GCRF_s"],
            angular_velocity_columns=[
                "w_B_GCRF_in_B_x",
                "w_B_GCRF_in_B_y",
                "w_B_GCRF_in_B_z",
            ],
        )

        assert profile is not None

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_position().get_coordinates(),
            np.array((1.0, 2.0, 3.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_velocity().get_coordinates(),
            np.array((4.0, 5.0, 6.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_attitude().to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 0.0, 1.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_angular_velocity(),
            np.array((0.0, 0.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_position()
            .get_coordinates(),
            np.array((11.0, 12.0, 13.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_velocity()
            .get_coordinates(),
            np.array((14.0, 15.0, 16.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_attitude()
            .to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 1.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0)).get_angular_velocity(),
            np.array((1.0, 1.0, 1.0)),
            atol=1e-8,
        )

        assert profile.get_state_at(epoch).get_frame() == Frame.GCRF()

    def test_generate_profile_from_dataframe_success_defined_columns_without_time(
        self,
        epoch: Instant,
        dataframe_indexed_timestamp: pd.DataFrame,
    ):
        profile: Profile = generate_profile_from_dataframe(
            dataframe=dataframe_indexed_timestamp,
            position_columns=["r_GCRF_x", "r_GCRF_y", "r_GCRF_z"],
            velocity_columns=["v_GCRF_x", "v_GCRF_y", "v_GCRF_z"],
            attitude_columns=["q_B_GCRF_x", "q_B_GCRF_y", "q_B_GCRF_z", "q_B_GCRF_s"],
            angular_velocity_columns=[
                "w_B_GCRF_in_B_x",
                "w_B_GCRF_in_B_y",
                "w_B_GCRF_in_B_z",
            ],
        )

        assert profile is not None

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_position().get_coordinates(),
            np.array((1.0, 2.0, 3.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_velocity().get_coordinates(),
            np.array((4.0, 5.0, 6.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_attitude().to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 0.0, 1.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_angular_velocity(),
            np.array((0.0, 0.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_position()
            .get_coordinates(),
            np.array((11.0, 12.0, 13.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_velocity()
            .get_coordinates(),
            np.array((14.0, 15.0, 16.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_attitude()
            .to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 1.0, 0.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0)).get_angular_velocity(),
            np.array((1.0, 1.0, 1.0)),
            atol=1e-8,
        )

        assert profile.get_state_at(epoch).get_frame() == Frame.GCRF()

    def test_generate_profile_from_dataframe_success_no_angular_velocity_columns(
        self,
        epoch: Instant,
        dataframe_indexed_timestamp: pd.DataFrame,
    ):
        dataframe_indexed_timestamp.drop(
            ["w_B_GCRF_in_B_x", "w_B_GCRF_in_B_y", "w_B_GCRF_in_B_z"],
            axis=1,
            inplace=True,
        )

        profile: Profile = generate_profile_from_dataframe(
            dataframe=dataframe_indexed_timestamp,
            position_columns=["r_GCRF_x", "r_GCRF_y", "r_GCRF_z"],
            velocity_columns=["v_GCRF_x", "v_GCRF_y", "v_GCRF_z"],
            attitude_columns=["q_B_GCRF_x", "q_B_GCRF_y", "q_B_GCRF_z", "q_B_GCRF_s"],
        )

        assert profile is not None

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_position().get_coordinates(),
            np.array((1.0, 2.0, 3.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_velocity().get_coordinates(),
            np.array((4.0, 5.0, 6.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch).get_attitude().to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 0.0, 1.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_position()
            .get_coordinates(),
            np.array((11.0, 12.0, 13.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_velocity()
            .get_coordinates(),
            np.array((14.0, 15.0, 16.0)),
            atol=1e-8,
        )

        np.testing.assert_allclose(
            profile.get_state_at(epoch + Duration.minutes(1.0))
            .get_attitude()
            .to_vector(Quaternion.Format.XYZS),
            np.array((0.0, 0.0, 1.0, 0.0)),
            atol=1e-8,
        )

        assert profile.get_state_at(epoch).get_frame() == Frame.GCRF()

    def test_generate_dataframe_from_profile_success(
        self,
        epoch: Instant,
        profile: Profile,
        dataframe_indexed_timestamp: pd.DataFrame,
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
        )

        pd.testing.assert_frame_equal(generated_dataframe, dataframe_indexed_timestamp)

    def test_generate_dataframe_from_profile_success_custom_columns(
        self,
        epoch: Instant,
        profile: Profile,
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            attitude_columns=["q_1", "q_2", "q_3", "q_4"],
            angular_velocity_columns=["w_1", "w_2", "w_3"],
        )

        assert list(generated_dataframe.columns) == [
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
            "q_1",
            "q_2",
            "q_3",
            "q_4",
            "w_1",
            "w_2",
            "w_3",
        ]

    def test_generate_dataframe_from_profile_success_custom_reference_frame(
        self,
        epoch: Instant,
        profile: Profile,
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
            reference_frame=Frame.ITRF(),
        )

        assert list(generated_dataframe.columns) == [
            "r_ITRF_x",
            "r_ITRF_y",
            "r_ITRF_z",
            "v_ITRF_x",
            "v_ITRF_y",
            "v_ITRF_z",
            "q_B_ITRF_x",
            "q_B_ITRF_y",
            "q_B_ITRF_z",
            "q_B_ITRF_s",
            "w_B_ITRF_in_B_x",
            "w_B_ITRF_in_B_y",
            "w_B_ITRF_in_B_z",
        ]

    def test_generate_dataframe_from_profile_success_set_time_index_disabled(
        self,
        epoch: Instant,
        profile: Profile,
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            attitude_columns=["q_1", "q_2", "q_3", "q_4"],
            angular_velocity_columns=["w_1", "w_2", "w_3"],
            set_time_index=False,
        )

        assert list(generated_dataframe.columns) == [
            "t",
            "r_1",
            "r_2",
            "r_3",
            "v_1",
            "v_2",
            "v_3",
            "q_1",
            "q_2",
            "q_3",
            "q_4",
            "w_1",
            "w_2",
            "w_3",
        ]
