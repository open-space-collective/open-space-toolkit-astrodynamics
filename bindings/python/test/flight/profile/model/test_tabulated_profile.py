# Apache License 2.0

from datetime import datetime

import pytest

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion
from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.profile.model import Tabulated as TabulatedModel


@pytest.fixture
def states() -> list[State]:
    return [
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
    ]


@pytest.fixture
def interpolator_type() -> Interpolator.Type:
    return Interpolator.Type.Linear


@pytest.fixture
def tabulated_model(
    states: list[State],
    interpolator_type: Interpolator.Type,
) -> TabulatedModel:
    return TabulatedModel(
        states=states,
        interpolator_type=interpolator_type,
    )


class TestTabulatedProfile:
    def test_constructors(
        self,
        tabulated_model: TabulatedModel,
    ):
        assert tabulated_model is not None
        assert isinstance(tabulated_model, TabulatedModel)

    def test_is_defined(
        self,
        tabulated_model: TabulatedModel,
    ):
        assert tabulated_model.is_defined() is True

    def test_get_interval(
        self,
        tabulated_model: TabulatedModel,
    ):
        interval = tabulated_model.get_interval()

        assert interval is not None

    def test_get_interpolator_type(
        self,
        tabulated_model: TabulatedModel,
    ):
        interpolator_type = tabulated_model.get_interpolator_type()

        assert interpolator_type is not None

    def test_calculate_state_at(
        self,
        tabulated_model: TabulatedModel,
        states: list[State],
    ):
        state: State = tabulated_model.calculate_state_at(states[0].get_instant())

        assert state is not None

    def test_get_axes_at(
        self,
        tabulated_model: TabulatedModel,
        states: list[State],
    ):
        axes = tabulated_model.get_axes_at(states[0].get_instant())

        assert axes is not None

    def test_get_body_frame(
        self,
        tabulated_model: TabulatedModel,
    ):
        if Frame.exists("test"):
            Frame.destruct("test")

        body_frame = tabulated_model.get_body_frame("test")

        assert body_frame is not None
