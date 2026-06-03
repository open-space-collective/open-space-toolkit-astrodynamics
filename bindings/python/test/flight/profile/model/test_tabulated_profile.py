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
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import AngularVelocity
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
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
def dense_states() -> list[State]:
    # Barycentric rational interpolation (the default) requires more than two points.
    return [
        State(
            instant=Instant.date_time(datetime(2020, 1, 1, 0, i, 0), Scale.UTC),
            position=Position.meters((float(i), 2.0 * i, 3.0 * i), Frame.GCRF()),
            velocity=Velocity.meters_per_second(
                (4.0 * i, 5.0 * i, 6.0 * i), Frame.GCRF()
            ),
            attitude=Quaternion.unit(),
            angular_velocity=(0.0, 0.0, 0.0),
            attitude_frame=Frame.GCRF(),
        )
        for i in range(5)
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

    def test_constructor_with_interpolation_types(
        self,
        dense_states: list[State],
    ):
        tabulated_model: TabulatedModel = TabulatedModel(
            states=dense_states,
            interpolation_types={
                CartesianPosition.default(): Interpolator.Type.BarycentricRational,
                CartesianVelocity.default(): Interpolator.Type.BarycentricRational,
                AngularVelocity.default(): Interpolator.Type.BarycentricRational,
            },
        )

        assert tabulated_model.is_defined()

    def test_constructor_with_interpolation_types_failure(
        self,
        states: list[State],
    ):
        # A coordinate subset present in the (reduced) states but missing from the map must raise.
        with pytest.raises(Exception):
            TabulatedModel(
                states=states,
                interpolation_types={
                    CartesianPosition.default(): Interpolator.Type.BarycentricRational,
                    CartesianVelocity.default(): Interpolator.Type.BarycentricRational,
                },
            )

    def test_default(
        self,
        dense_states: list[State],
    ):
        tabulated_model: TabulatedModel = TabulatedModel.default(dense_states)

        assert tabulated_model.is_defined()
        assert (
            tabulated_model.get_interpolator_type()
            == Interpolator.Type.BarycentricRational
        )

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

    def test_construct_body_frame(
        self,
        tabulated_model: TabulatedModel,
    ):
        if Frame.exists("test"):
            Frame.destruct("test")

        body_frame = tabulated_model.construct_body_frame("test")

        assert body_frame is not None
