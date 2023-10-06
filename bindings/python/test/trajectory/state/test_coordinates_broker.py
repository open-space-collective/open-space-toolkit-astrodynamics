# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state import CoordinatesBroker, CoordinatesSubset


@pytest.fixture
def coordinates_subsets() -> list:
    return [CoordinatesSubset("Subset1", 2), CoordinatesSubset("Subset2", 3)]


@pytest.fixture
def coordinates_broker(coordinates_subsets) -> CoordinatesBroker:
    return CoordinatesBroker(coordinates_subsets)


@pytest.fixture
def coordinates() -> list[float]:
    return [1.0, 2.0, 3.0, 4.0, 5.0]


class TestCoordinatesBroker:
    def test_constructor(self, coordinates_broker: CoordinatesBroker):
        assert coordinates_broker is not None

    def test_eq(self, coordinates_broker: CoordinatesBroker):
        assert coordinates_broker == coordinates_broker

    def test_ne(self, coordinates_broker: CoordinatesBroker):
        assert (coordinates_broker != coordinates_broker) == False

    def test_access_subsets(
        self, coordinates_broker: CoordinatesBroker, coordinates_subsets: list
    ):
        assert coordinates_broker.access_subsets() == coordinates_subsets

    def test_get_number_of_coordinates(self, coordinates_broker: CoordinatesBroker):
        assert coordinates_broker.get_number_of_coordinates() == 5

    def test_get_number_of_subsets(self, coordinates_broker: CoordinatesBroker):
        assert coordinates_broker.get_number_of_subsets() == 2

    def test_get_subsets(
        self, coordinates_broker: CoordinatesBroker, coordinates_subsets: list
    ):
        assert coordinates_broker.get_subsets() == coordinates_subsets

    def test_add_subset(self, coordinates_broker: CoordinatesBroker):
        new_subset = CoordinatesSubset("NewSubset", 4)
        number_of_coordinates = coordinates_broker.get_number_of_coordinates()
        assert coordinates_broker.add_subset(new_subset) == number_of_coordinates
        assert coordinates_broker.has_subset(new_subset)

    def test_has_subset(
        self, coordinates_broker: CoordinatesBroker, coordinates_subsets: list
    ):
        assert coordinates_broker.has_subset(coordinates_subsets[0])

    def test_extract_coordinates(
        self,
        coordinates_broker: CoordinatesBroker,
        coordinates: list[float],
        coordinates_subsets: list[CoordinatesSubset],
    ):
        assert (
            coordinates_broker.extract_coordinates(coordinates, coordinates_subsets[0])
            == [1.0, 2.0]
        ).all()
        assert (
            coordinates_broker.extract_coordinates(coordinates, coordinates_subsets[1])
            == [3.0, 4.0, 5.0]
        ).all()

        assert (
            coordinates_broker.extract_coordinates(coordinates, coordinates_subsets)
            == [1.0, 2.0, 3.0, 4.0, 5.0]
        ).all()
