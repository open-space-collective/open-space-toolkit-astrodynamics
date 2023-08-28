# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state import CoordinatesSubset


@pytest.fixture
def name() -> str:
    return "MySubset"


@pytest.fixture
def size() -> int:
    return 3


@pytest.fixture
def coordinates_subset(name: str, size: int) -> CoordinatesSubset:
    return CoordinatesSubset(name, size)


class TestCoordinatesSubset:
    def test_constructor(self, coordinates_subset: CoordinatesSubset):
        assert coordinates_subset is not None

    def test_eq(self, coordinates_subset: CoordinatesSubset):
        assert coordinates_subset == coordinates_subset

    def test_ne(self, coordinates_subset: CoordinatesSubset):
        assert (coordinates_subset != coordinates_subset) == False

    def test_get_id(self, coordinates_subset: CoordinatesSubset):
        assert coordinates_subset.get_id() is not None

    def test_get_name(self, coordinates_subset: CoordinatesSubset, name: str):
        assert coordinates_subset.get_name() == name

    def test_get_size(self, coordinates_subset: CoordinatesSubset, size: int):
        assert coordinates_subset.get_size() == size

    def test_mass(self):
        assert CoordinatesSubset.mass() is not None
