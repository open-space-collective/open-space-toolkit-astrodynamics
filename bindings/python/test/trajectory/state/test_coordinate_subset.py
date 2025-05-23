# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state import CoordinateSubset


@pytest.fixture
def name() -> str:
    return "MySubset"


@pytest.fixture
def size() -> int:
    return 3


@pytest.fixture
def coordinate_subset(name: str, size: int) -> CoordinateSubset:
    return CoordinateSubset(name, size)


class TestCoordinateSubset:
    def test_constructor(self, coordinate_subset: CoordinateSubset):
        assert coordinate_subset is not None

    def test_eq(self, coordinate_subset: CoordinateSubset):
        assert coordinate_subset == coordinate_subset

    def test_ne(self, coordinate_subset: CoordinateSubset):
        assert (coordinate_subset != coordinate_subset) == False

    def test_hash(self, coordinate_subset: CoordinateSubset):
        assert hash(coordinate_subset) is not None

    def test_get_id(self, coordinate_subset: CoordinateSubset):
        assert coordinate_subset.get_id() is not None

    def test_get_name(self, coordinate_subset: CoordinateSubset, name: str):
        assert coordinate_subset.get_name() == name

    def test_get_size(self, coordinate_subset: CoordinateSubset, size: int):
        assert coordinate_subset.get_size() == size

    def test_mass(self):
        assert CoordinateSubset.mass() is not None

    def test_surface_area(self):
        assert CoordinateSubset.surface_area() is not None

    def test_drag_coefficient(self):
        assert CoordinateSubset.drag_coefficient() is not None

    def test_mass_flow_rate(self):
        assert CoordinateSubset.mass_flow_rate() is not None

    def test_ballistic_coefficient(self):
        assert CoordinateSubset.ballistic_coefficient() is not None
