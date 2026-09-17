# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.estimator import CovarianceMatrix
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
def covariance_matrix(
    instant: Instant,
    coordinates: np.ndarray,
    frame: Frame,
    coordinate_subsets: list[CoordinateSubset],
) -> CovarianceMatrix:
    return CovarianceMatrix(instant, coordinates, frame, coordinate_subsets)


class TestCovarianceMatrix:
    def test_constructor(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        assert covariance_matrix is not None
        assert isinstance(covariance_matrix, CovarianceMatrix)
        assert covariance_matrix.is_defined()

    def test_comparators(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        assert (covariance_matrix == covariance_matrix) is True
        assert (covariance_matrix != covariance_matrix) is False

    def test_operators(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        assert isinstance(covariance_matrix + covariance_matrix, CovarianceMatrix)
        assert isinstance(covariance_matrix - covariance_matrix, CovarianceMatrix)
        assert np.allclose(
            (covariance_matrix + covariance_matrix).get_coordinates(), 2.0 * np.eye(6)
        )
        assert np.allclose(
            (covariance_matrix - covariance_matrix).get_coordinates(), np.zeros((6, 6))
        )

    def test_string_representation(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        assert str(covariance_matrix) is not None
        assert repr(covariance_matrix) is not None

    def test_is_defined(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        assert covariance_matrix.is_defined() is True
        assert CovarianceMatrix.undefined().is_defined() is False

    def test_accessors(
        self,
        covariance_matrix: CovarianceMatrix,
        instant: Instant,
        frame: Frame,
        coordinates: np.ndarray,
    ):
        assert covariance_matrix.access_instant() == instant
        assert covariance_matrix.access_frame() == frame
        assert np.allclose(covariance_matrix.access_coordinates(), coordinates)

    def test_getters(
        self,
        covariance_matrix: CovarianceMatrix,
        instant: Instant,
        frame: Frame,
        coordinates: np.ndarray,
        coordinate_subsets: list[CoordinateSubset],
    ):
        assert covariance_matrix.get_size() == 6
        assert covariance_matrix.get_instant() == instant
        assert covariance_matrix.get_frame() == frame
        assert np.allclose(covariance_matrix.get_coordinates(), coordinates)
        assert covariance_matrix.get_coordinate_subsets() == coordinate_subsets

    def test_extract_coordinates(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        position_coordinates = covariance_matrix.extract_coordinate(
            CartesianPosition.default()
        )
        velocity_coordinates = covariance_matrix.extract_coordinate(
            CartesianVelocity.default()
        )

        assert np.allclose(position_coordinates, np.eye(3))
        assert np.allclose(velocity_coordinates, np.eye(3))
        assert np.allclose(
            covariance_matrix.extract_coordinates(
                [CartesianPosition.default(), CartesianVelocity.default()]
            ),
            np.eye(6),
        )

    def test_rotate(
        self,
        covariance_matrix: CovarianceMatrix,
        frame: Frame,
    ):
        assert covariance_matrix.rotate(frame) == covariance_matrix

        rotated_covariance_matrix = covariance_matrix.rotate(Frame.ITRF())

        assert rotated_covariance_matrix.get_frame() == Frame.ITRF()
        assert np.allclose(rotated_covariance_matrix.get_coordinates(), np.eye(6))

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

        covariance_matrix = CovarianceMatrix(
            instant,
            coordinates,
            frame,
            [CartesianPosition.default()],
        )

        diagonalized_covariance_matrix = covariance_matrix.diagonalize()

        assert np.allclose(
            diagonalized_covariance_matrix.get_coordinates(),
            np.diag([1.0, 2.0, 3.0]),
        )

    def test_reduce(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        reduced_covariance_matrix = covariance_matrix.reduce(
            [CartesianPosition.default()]
        )

        assert reduced_covariance_matrix.get_size() == 3
        assert np.allclose(reduced_covariance_matrix.get_coordinates(), np.eye(3))

    def test_scale(
        self,
        covariance_matrix: CovarianceMatrix,
    ):
        scaled_covariance_matrix = covariance_matrix.scale(2.0)

        assert np.allclose(scaled_covariance_matrix.get_coordinates(), 2.0 * np.eye(6))

    def test_undefined(self):
        covariance_matrix = CovarianceMatrix.undefined()

        assert isinstance(covariance_matrix, CovarianceMatrix)
        assert covariance_matrix.is_defined() is False

    def test_from_sigmas(
        self,
        instant: Instant,
        frame: Frame,
    ):
        position_covariance_matrix = CovarianceMatrix.from_position_sigmas(
            instant,
            np.array([1.0, 2.0, 3.0]),
            frame,
        )

        assert position_covariance_matrix.get_size() == 3
        assert np.allclose(
            position_covariance_matrix.get_coordinates(),
            np.diag([1.0, 4.0, 9.0]),
        )

        position_velocity_covariance_matrix = (
            CovarianceMatrix.from_position_velocity_sigmas(
                instant,
                np.array([1.0, 2.0, 3.0]),
                np.array([4.0, 5.0, 6.0]),
                frame,
            )
        )

        assert position_velocity_covariance_matrix.get_size() == 6
        assert np.allclose(
            position_velocity_covariance_matrix.get_coordinates(),
            np.diag([1.0, 4.0, 9.0, 16.0, 25.0, 36.0]),
        )
