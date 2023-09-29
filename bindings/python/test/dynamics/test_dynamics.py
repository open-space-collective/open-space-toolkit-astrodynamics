# Apache License 2.0

import pytest

from ostk.physics import Environment

from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition


@pytest.fixture
def name() -> str:
    return "MyDynamics"


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def dynamics(name: str) -> Dynamics:
    class MyDynamics(Dynamics):
        def is_defined():
            return True

        def get_read_coordinates_subsets(self):
            return [CartesianPosition.default()]

        def get_write_coordinates_subsets(self):
            return [CartesianPosition.default()]

        def compute_contribution(self, instant, state_vector, frame):
            return state_vector

    return MyDynamics(name)


class TestDynamics:
    def test_subclass(self, dynamics: Dynamics):
        assert dynamics is not None

    def test_get_name(self, dynamics: Dynamics, name: str):
        assert dynamics.get_name() == name

    def test_from_environment(self, environment: Environment):
        dynamics: list[Dynamics] = Dynamics.from_environment(environment)

        assert dynamics is not None
        assert isinstance(dynamics, list)
