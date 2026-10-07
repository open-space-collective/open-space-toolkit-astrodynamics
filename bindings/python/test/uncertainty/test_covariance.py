# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.uncertainty import Covariance
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [
        CartesianPosition.default(),
        CartesianVelocity.default(),
    ]


@pytest.fixture
def coordinates() -> np.ndarray:
    return np.eye(6)


@pytest.fixture
def covariance(
    instant: Instant,
    coordinates: np.ndarray,
    frame: Frame,
    coordinate_subsets: list[CoordinateSubset],
) -> Covariance:
    return Covariance(instant, coordinates, frame, coordinate_subsets)


class TestCovariance:
    def test_constructor(
        self,
        covariance: Covariance,
    ):
        assert covariance is not None
        assert isinstance(covariance, Covariance)

    def test_operators(
        self,
        covariance: Covariance,
    ):
        assert isinstance(covariance + covariance, Covariance)
        assert isinstance(covariance - covariance, Covariance)
        assert np.allclose((covariance + covariance).get_coordinates(), 2.0 * np.eye(6))
        assert np.allclose((covariance - covariance).get_coordinates(), np.zeros((6, 6)))

    def test_string_representation(
        self,
        covariance: Covariance,
    ):
        assert str(covariance) is not None
        assert repr(covariance) is not None

    def test_getters(
        self,
        covariance: Covariance,
        instant: Instant,
        frame: Frame,
        coordinates: np.ndarray,
        coordinate_subsets: list[CoordinateSubset],
    ):
        assert covariance.get_size() == 6
        assert covariance.get_instant() == instant
        assert covariance.get_frame() == frame
        assert np.allclose(covariance.get_coordinates(), coordinates)
        assert covariance.get_coordinate_subsets() == coordinate_subsets

    def test_extract_coordinates(
        self,
        covariance: Covariance,
    ):
        position_coordinates = covariance.extract_coordinate(CartesianPosition.default())
        velocity_coordinates = covariance.extract_coordinate(CartesianVelocity.default())

        assert np.allclose(position_coordinates, np.eye(3))
        assert np.allclose(velocity_coordinates, np.eye(3))
        assert np.allclose(
            covariance.extract_coordinates(
                [CartesianPosition.default(), CartesianVelocity.default()]
            ),
            np.eye(6),
        )

    def test_rotate(
        self,
        covariance: Covariance,
        frame: Frame,
    ):
        assert np.allclose(
            covariance.rotate(frame).get_coordinates(),
            covariance.get_coordinates(),
        )

        rotated_covariance = covariance.rotate(Frame.ITRF())

        assert rotated_covariance.get_frame() == Frame.ITRF()
        assert np.allclose(rotated_covariance.get_coordinates(), np.eye(6))

    def test_diagonalize(
        self,
        instant: Instant,
        frame: Frame,
    ):
        coordinates = np.array(
            [
                [1.0, 0.2, 0.3],
                [0.2, 2.0, 0.4],
                [0.3, 0.4, 3.0],
            ]
        )

        covariance = Covariance(
            instant,
            coordinates,
            frame,
            [CartesianPosition.default()],
        )

        diagonalized_covariance = covariance.diagonalize()

        assert np.allclose(
            diagonalized_covariance.get_coordinates(),
            np.diag([1.0, 2.0, 3.0]),
        )

    def test_reduce(
        self,
        covariance: Covariance,
    ):
        reduced_covariance = covariance.reduce([CartesianPosition.default()])

        assert reduced_covariance.get_size() == 3
        assert np.allclose(reduced_covariance.get_coordinates(), np.eye(3))

    def test_scale(
        self,
        covariance: Covariance,
    ):
        scaled_covariance = covariance.scale(2.0)

        assert np.allclose(scaled_covariance.get_coordinates(), 2.0 * np.eye(6))

    def test_from_sigmas(
        self,
        instant: Instant,
        frame: Frame,
    ):
        position_covariance = Covariance.from_position_sigmas(
            instant,
            np.array([1.0, 2.0, 3.0]),
            frame,
        )

        assert position_covariance.get_size() == 3
        assert np.allclose(
            position_covariance.get_coordinates(),
            np.diag([1.0, 4.0, 9.0]),
        )

        position_velocity_covariance = Covariance.from_position_velocity_sigmas(
            instant,
            np.array([1.0, 2.0, 3.0]),
            np.array([4.0, 5.0, 6.0]),
            frame,
        )

        assert position_velocity_covariance.get_size() == 6
        assert np.allclose(
            position_velocity_covariance.get_coordinates(),
            np.diag([1.0, 4.0, 9.0, 16.0, 25.0, 36.0]),
        )
