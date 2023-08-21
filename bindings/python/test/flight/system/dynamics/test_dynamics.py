# Apache License 2.0

import pytest

from ostk.astrodynamics.flight.system import Dynamics


@pytest.fixture
def name() -> str:
    return "MyDynamics"



@pytest.fixture
def dynamics(name: str) -> Dynamics:
    class MyDynamics(Dynamics):
        def get_read_coordinates_subsets(self):
            return []

        def get_write_coordinates_subsets(self):
            return []

        def compute_contribution(self, instant, state_vector, frame):
            return state_vector

    return MyDynamics(name)


class TestDynamics:
    def test_subclass(self, dynamics: Dynamics):
        assert dynamics is not None

    def test_get_name(self, dynamics: Dynamics, name: str):
        assert dynamics.get_name() == name
