# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state import CoordinateBroker, CoordinateSubset


@pytest.fixture
def coordinate_subsets() -> list:
    return [CoordinateSubset("Subset1", 2), CoordinateSubset("Subset2", 3)]


@pytest.fixture
def coordinates_broker(coordinate_subsets) -> CoordinateBroker:
    return CoordinateBroker(coordinate_subsets)


@pytest.fixture
def coordinates() -> list[float]:
    return [1.0, 2.0, 3.0, 4.0, 5.0]


class TestCoordinateBroker:
    def test_constructor(self, coordinates_broker: CoordinateBroker):
        assert coordinates_broker is not None

    def test_eq(self, coordinates_broker: CoordinateBroker):
        assert coordinates_broker == coordinates_broker

    def test_ne(self, coordinates_broker: CoordinateBroker):
        assert (coordinates_broker != coordinates_broker) == False

    def test_access_subsets(
        self, coordinates_broker: CoordinateBroker, coordinate_subsets: list
    ):
        assert coordinates_broker.access_subsets() == coordinate_subsets

    def test_get_number_of_coordinates(self, coordinates_broker: CoordinateBroker):
        assert coordinates_broker.get_number_of_coordinates() == 5

    def test_get_number_of_subsets(self, coordinates_broker: CoordinateBroker):
        assert coordinates_broker.get_number_of_subsets() == 2

    def test_get_subsets(
        self, coordinates_broker: CoordinateBroker, coordinate_subsets: list
    ):
        assert coordinates_broker.get_subsets() == coordinate_subsets

    def test_add_subset(self, coordinates_broker: CoordinateBroker):
        new_subset = CoordinateSubset("NewSubset", 4)
        number_of_coordinates = coordinates_broker.get_number_of_coordinates()
        assert coordinates_broker.add_subset(new_subset) == number_of_coordinates
        assert coordinates_broker.has_subset(new_subset)

    def test_has_subset(
        self, coordinates_broker: CoordinateBroker, coordinate_subsets: list
    ):
        assert coordinates_broker.has_subset(coordinate_subsets[0])

    def test_extract_coordinate(
        self,
        coordinates_broker: CoordinateBroker,
        coordinates: list[float],
        coordinate_subsets: list[CoordinateSubset],
    ):
        assert (
            coordinates_broker.extract_coordinate(coordinates, coordinate_subsets[0])
            == [1.0, 2.0]
        ).all()
        assert (
            coordinates_broker.extract_coordinate(coordinates, coordinate_subsets[1])
            == [3.0, 4.0, 5.0]
        ).all()

    def test_extract_coordinates(
        self,
        coordinates_broker: CoordinateBroker,
        coordinates: list[float],
        coordinate_subsets: list[CoordinateSubset],
    ):
        assert (
            coordinates_broker.extract_coordinates(coordinates, coordinate_subsets)
            == [1.0, 2.0, 3.0, 4.0, 5.0]
        ).all()
