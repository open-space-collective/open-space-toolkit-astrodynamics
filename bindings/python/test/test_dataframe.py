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
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory import StateBuilder
from ostk.astrodynamics.flight import Profile

from ostk.mathematics.curve_fitting import Interpolator

from datetime import datetime

from ostk.astrodynamics.dataframe import generate_states_from_dataframe
from ostk.astrodynamics.dataframe import generate_dataframe_from_states
from ostk.astrodynamics.dataframe import generate_profile_from_dataframe
from ostk.astrodynamics.dataframe import generate_dataframe_from_profile
from ostk.astrodynamics.dataframe import compare_states_to_states
from ostk.astrodynamics.dataframe import compare_orbit_to_states
from ostk.astrodynamics.dataframe import generate_orbit_from_dataframe


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

    @pytest.fixture
    def orbit_state_with_properties(
        self,
        instant: Instant,
        position: Position,
        velocity: Velocity,
    ) -> State:
        state_builder = StateBuilder(
            frame=Frame.GCRF(),
            coordinate_subsets=[
                CartesianPosition.default(),
                CartesianVelocity.default(),
                CoordinateSubset.mass(),
                CoordinateSubset.drag_coefficient(),
                CoordinateSubset.surface_area(),
            ],
        )

        return state_builder.build(
            instant=instant,
            coordinates=[
                *position.get_coordinates(),
                *velocity.get_coordinates(),
                100.0,  # mass in kg
                2.2,  # drag coefficient
                10.5,  # surface area in m²
            ],
        )

    @pytest.fixture
    def orbit_states_with_properties(
        self, orbit_state_with_properties: State
    ) -> list[State]:
        return [
            orbit_state_with_properties,
            orbit_state_with_properties,
            orbit_state_with_properties,
        ]

    @pytest.fixture
    def orbit_dataframe_with_properties(self, instant: Instant) -> pd.DataFrame:
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
                    "mass": 100.0,
                    "drag_coefficient": 2.2,
                    "surface_area": 10.5,
                },
                {
                    "Timestamp": coerce_to_datetime(instant + Duration.minutes(1.0)),
                    "r_GCRF_x": 11.0,
                    "r_GCRF_y": 12.0,
                    "r_GCRF_z": 13.0,
                    "v_GCRF_x": 14.0,
                    "v_GCRF_y": 15.0,
                    "v_GCRF_z": 16.0,
                    "mass": 99.5,
                    "drag_coefficient": 2.2,
                    "surface_area": 10.5,
                },
            ]
        )

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

    def test_generate_states_from_dataframe_with_properties_success(
        self,
        orbit_dataframe_with_properties: pd.DataFrame,
    ):
        states: list[State] = generate_states_from_dataframe(
            dataframe=orbit_dataframe_with_properties,
            reference_frame=Frame.GCRF(),
        )

        for state in states:
            assert (
                len(state.get_coordinates()) == 9
            )  # 3 position + 3 velocity + mass + drag_coefficient + surface_area
            assert state.has_subset(CoordinateSubset.mass())
            assert state.has_subset(CoordinateSubset.drag_coefficient())
            assert state.has_subset(CoordinateSubset.surface_area())

    def test_generate_dataframe_from_states_with_properties_success(
        self,
        orbit_states_with_properties: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=orbit_states_with_properties,
        )

        assert "mass" in generated_dataframe.columns
        assert "drag_coefficient" in generated_dataframe.columns
        assert "surface_area" in generated_dataframe.columns

        # Verify the values are correct
        assert generated_dataframe["mass"].iloc[0] == 100.0
        assert generated_dataframe["drag_coefficient"].iloc[0] == 2.2
        assert generated_dataframe["surface_area"].iloc[0] == 10.5

    def test_generate_dataframe_from_states_with_custom_property_columns(
        self,
        orbit_states_with_properties: list[State],
    ):
        generated_dataframe: pd.DataFrame = generate_dataframe_from_states(
            states=orbit_states_with_properties,
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            mass_column="spacecraft_mass",
            drag_coefficient_column="cd",
            surface_area_column="area",
        )

        assert "spacecraft_mass" in generated_dataframe.columns
        assert "cd" in generated_dataframe.columns
        assert "area" in generated_dataframe.columns

        # Verify the values are correct
        assert generated_dataframe["spacecraft_mass"].iloc[0] == 100.0
        assert generated_dataframe["cd"].iloc[0] == 2.2
        assert generated_dataframe["area"].iloc[0] == 10.5


class TestProfileDataframe:
    @pytest.fixture
    def environment(self) -> Environment:
        return Environment.default()

    @pytest.fixture
    def epoch(self) -> Instant:
        return Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC)

    @pytest.fixture
    def dataframe(
        self,
        epoch: Instant,
    ) -> pd.DataFrame:
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
    def profile(
        self,
        dataframe: pd.DataFrame,
    ) -> Profile:
        return generate_profile_from_dataframe(
            dataframe=dataframe,
        )

    @pytest.fixture
    def dataframe_indexed_timestamp(
        self,
        dataframe: pd.DataFrame,
    ) -> pd.DataFrame:
        dataframe.set_index("Timestamp", inplace=True)
        return dataframe

    @pytest.fixture
    def dataframe_with_properties(
        self,
        epoch: Instant,
    ) -> pd.DataFrame:
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
                    "mass": 100.0,
                    "drag_coefficient": 2.2,
                    "surface_area": 10.5,
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
                    "mass": 99.5,
                    "drag_coefficient": 2.2,
                    "surface_area": 10.5,
                },
            ]
        )

    @pytest.fixture
    def dataframe_with_properties_indexed_timestamp(
        self,
        dataframe_with_properties: pd.DataFrame,
    ) -> pd.DataFrame:
        dataframe_with_properties.set_index("Timestamp", inplace=True)
        return dataframe_with_properties

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

    def test_generate_profile_from_dataframe_with_properties_success(
        self,
        epoch: Instant,
        dataframe_with_properties: pd.DataFrame,
    ):
        profile: Profile = generate_profile_from_dataframe(
            dataframe=dataframe_with_properties,
        )

        assert profile is not None
        state = profile.get_state_at(epoch)

        assert state.has_subset(CoordinateSubset.mass())
        assert state.has_subset(CoordinateSubset.drag_coefficient())
        assert state.has_subset(CoordinateSubset.surface_area())

        # Verify extracted coordinates are correct
        assert state.extract_coordinate(CoordinateSubset.mass())[0] == 100.0
        assert state.extract_coordinate(CoordinateSubset.drag_coefficient())[0] == 2.2
        assert state.extract_coordinate(CoordinateSubset.surface_area())[0] == 10.5

    def test_generate_dataframe_from_profile_with_properties_success(
        self,
        epoch: Instant,
        dataframe_with_properties_indexed_timestamp: pd.DataFrame,
    ):
        # Create a profile with properties
        profile_with_props = generate_profile_from_dataframe(
            dataframe=dataframe_with_properties_indexed_timestamp,
        )

        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile_with_props,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
        )

        assert "mass" in generated_dataframe.columns
        assert "drag_coefficient" in generated_dataframe.columns
        assert "surface_area" in generated_dataframe.columns

        # Verify the values
        assert generated_dataframe["mass"].iloc[0] == 100.0
        assert generated_dataframe["drag_coefficient"].iloc[0] == 2.2
        assert generated_dataframe["surface_area"].iloc[0] == 10.5

    def test_generate_dataframe_from_profile_with_custom_property_columns(
        self,
        epoch: Instant,
        dataframe_with_properties_indexed_timestamp: pd.DataFrame,
    ):
        # Create a profile with properties
        profile_with_props = generate_profile_from_dataframe(
            dataframe=dataframe_with_properties_indexed_timestamp,
        )

        generated_dataframe: pd.DataFrame = generate_dataframe_from_profile(
            profile=profile_with_props,
            instants=[
                epoch,
                epoch + Duration.minutes(1.0),
            ],
            time_column="t",
            position_columns=["r_1", "r_2", "r_3"],
            velocity_columns=["v_1", "v_2", "v_3"],
            attitude_columns=["q_1", "q_2", "q_3", "q_4"],
            angular_velocity_columns=["w_1", "w_2", "w_3"],
            mass_column="spacecraft_mass",
            drag_coefficient_column="cd",
            surface_area_column="area",
            set_time_index=False,
        )

        assert "spacecraft_mass" in generated_dataframe.columns
        assert "cd" in generated_dataframe.columns
        assert "area" in generated_dataframe.columns

        # Verify the values
        assert generated_dataframe["spacecraft_mass"].iloc[0] == 100.0
        assert generated_dataframe["cd"].iloc[0] == 2.2
        assert generated_dataframe["area"].iloc[0] == 10.5


class TestCompareUtilities:
    @pytest.fixture
    def instant(self) -> Instant:
        return Instant.date_time(DateTime.parse("2024-01-29T00:00:00"), Scale.UTC)

    @pytest.fixture
    def frame(self) -> Frame:
        return Frame.GCRF()

    @pytest.fixture
    def position_1(self, frame: Frame) -> Position:
        return Position.meters([7000000.0, 0.0, 0.0], frame)

    @pytest.fixture
    def velocity_1(self, frame: Frame) -> Velocity:
        return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)

    @pytest.fixture
    def position_2(self, frame: Frame) -> Position:
        return Position.meters([7000100.0, 0.0, 0.0], frame)

    @pytest.fixture
    def velocity_2(self, frame: Frame) -> Velocity:
        return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)

    @pytest.fixture
    def state_1(
        self, instant: Instant, position_1: Position, velocity_1: Velocity
    ) -> State:
        return State(instant=instant, position=position_1, velocity=velocity_1)

    @pytest.fixture
    def state_2(
        self, instant: Instant, position_2: Position, velocity_2: Velocity
    ) -> State:
        return State(instant=instant, position=position_2, velocity=velocity_2)

    @pytest.fixture
    def states_1(self, state_1: State) -> list[State]:
        return [state_1, state_1, state_1]

    @pytest.fixture
    def states_2(self, state_2: State) -> list[State]:
        return [state_2, state_2, state_2]

    @pytest.fixture
    def orbit_dataframe(self, instant: Instant) -> pd.DataFrame:
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

    def test_compare_states_to_states_identical_states(
        self, states_1: list[State]
    ) -> None:
        """Test comparing identical states returns zero residuals."""
        residuals = compare_states_to_states(states_1, states_1)

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
        self, states_1: list[State], states_2: list[State]
    ) -> None:
        """Test comparing different states returns non-zero residuals."""
        residuals = compare_states_to_states(states_1, states_2)

        assert len(residuals) == 3
        # Expected difference is 100m in x-direction
        expected_residual = 100.0
        for residual in residuals:
            assert isinstance(residual, dict)
            assert residual["dr"] == pytest.approx(expected_residual, rel=1e-6)
            assert residual["dr_x"] == pytest.approx(expected_residual, rel=1e-6)
            assert residual["dr_y"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dr_z"] == pytest.approx(0.0, abs=1e-10)
            assert residual["dv"] == pytest.approx(0.0, abs=1e-10)

    def test_compare_states_to_states_returns_list_of_dicts(
        self, states_1: list[State], states_2: list[State]
    ) -> None:
        """Test that compare_states_to_states returns a list of dictionaries."""
        residuals = compare_states_to_states(states_1, states_2)

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
        self, orbit_dataframe: pd.DataFrame
    ) -> None:
        """Test that compare_orbit_to_states returns a list of dictionaries."""
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe, interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = compare_orbit_to_states(orbit, states)

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
        self, orbit_dataframe: pd.DataFrame
    ) -> None:
        """Test comparing orbit to its own states returns near-zero residuals."""
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe, interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = compare_orbit_to_states(orbit, states)

        assert len(result) == 2
        for entry in result:
            # Should be very small due to numerical precision and interpolation
            assert entry["dr"] < 1.0  # Less than 1 meter
            assert entry["dv"] < 0.01  # Less than 1 cm/s

    def test_compare_orbit_to_states_datetime_ordering(
        self, orbit_dataframe: pd.DataFrame, instant: Instant
    ) -> None:
        """Test that timestamps in result match the input states."""
        orbit = generate_orbit_from_dataframe(
            orbit_dataframe, interpolation_type=Interpolator.Type.Linear
        )
        states = generate_states_from_dataframe(orbit_dataframe)

        result = compare_orbit_to_states(orbit, states)

        expected_datetimes = [
            instant.get_date_time(Scale.UTC),
            (instant + Duration.seconds(60.0)).get_date_time(Scale.UTC),
        ]

        for i, entry in enumerate(result):
            # Both are Python datetime objects, direct comparison should work
            assert entry["timestamp"] == expected_datetimes[i]
