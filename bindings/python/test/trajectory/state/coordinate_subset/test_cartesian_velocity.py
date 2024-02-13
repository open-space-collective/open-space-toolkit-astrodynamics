# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinateBroker, CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianVelocity,
    CartesianPosition,
)


@pytest.fixture
def name() -> str:
    return "Position"


@pytest.fixture
def cartesian_position() -> CartesianPosition:
    return CartesianPosition.default()


@pytest.fixture
def cartesian_velocity() -> CartesianVelocity:
    return CartesianVelocity.default()


@pytest.fixture
def coordinate_subsets(
    cartesian_position: CartesianPosition, cartesian_velocity: CartesianVelocity
) -> list[CoordinateSubset]:
    return [cartesian_position, cartesian_velocity]


@pytest.fixture
def instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinate_broker(
    coordinate_subsets: list[CoordinateSubset],
) -> CoordinateBroker:
    return CoordinateBroker(coordinate_subsets)


@pytest.fixture
def coordinates() -> list[float]:
    return [7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126]


@pytest.fixture
def another_coordinates() -> list[float]:
    return [0.0, 7000000.0, 0.0, 5335.865450622126, 0.0, 0.0]


class TestCartesianVelocity:
    def test_constructor(self, name: str):
        assert CartesianVelocity(CartesianPosition.default(), name) is not None

    def test_add(
        self,
        cartesian_velocity: CartesianVelocity,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            cartesian_velocity.add(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [5335.865450622126, 5335.865450622126, 5335.865450622126]
        )

    def test_subtract(
        self,
        cartesian_velocity: CartesianVelocity,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            cartesian_velocity.subtract(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [-5335.865450622126, 5335.865450622126, 5335.865450622126]
        )

    def test_in_frame(
        self,
        cartesian_velocity: CartesianVelocity,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        for value, expected in zip(
            cartesian_velocity.in_frame(
                instant, coordinates, frame, Frame.ITRF(), coordinate_broker
            ),
            [-4749.36551256577, 854.163395375881, 5335.71857543495],
        ):
            assert value == pytest.approx(expected, rel=1e-14)
