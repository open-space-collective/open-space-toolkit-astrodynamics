# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinatesBroker, CoordinatesSubset
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition


@pytest.fixture
def name() -> str:
    return "Position"


@pytest.fixture
def cartesian_position() -> CartesianPosition:
    return CartesianPosition.default()


@pytest.fixture
def coordinates_subsets(cartesian_position: CartesianPosition) -> list[CoordinatesSubset]:
    return [cartesian_position]


@pytest.fixture
def instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinates_broker(coordinates_subsets: list[CoordinatesSubset]) -> CoordinatesBroker:
    return CoordinatesBroker(coordinates_subsets)


@pytest.fixture
def coordinates() -> list[float]:
    return [7000000.0, 0.0, 0.0]


@pytest.fixture
def another_coordinates() -> list[float]:
    return [0.0, 7000000.0, 0.0]


class TestCoordinatesSubset:
    def test_constructor(self, name: str):
        assert CartesianPosition(name) is not None

    def test_add(
        self,
        cartesian_position: CartesianPosition,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinates_broker: CoordinatesBroker,
    ):
        assert all(
            cartesian_position.add(
                instant, coordinates, another_coordinates, frame, coordinates_broker
            )
            == [7000000.0, 7000000.0, 0.0]
        )

    def test_subtract(
        self,
        cartesian_position: CartesianPosition,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinates_broker: CoordinatesBroker,
    ):
        assert all(
            cartesian_position.subtract(
                instant, coordinates, another_coordinates, frame, coordinates_broker
            )
            == [7000000.0, -7000000.0, 0.0]
        )

    def test_in_frame(
        self,
        cartesian_position: CartesianPosition,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinates_broker: CoordinatesBroker,
    ):
        for value, expected in zip(
            cartesian_position.in_frame(
                instant, coordinates, frame, Frame.ITRF(), coordinates_broker
            ),
            [1238864.12746338, 6889500.39136482, -176.262107699686],
        ):
            assert value == pytest.approx(expected, rel=1e-14)
