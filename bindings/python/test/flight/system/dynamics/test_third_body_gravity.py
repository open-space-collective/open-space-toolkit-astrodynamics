# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Moon

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import Dynamics
from ostk.astrodynamics.flight.system.dynamics import ThirdBodyGravity


@pytest.fixture
def moon() -> Moon:
    return Moon.spherical()


@pytest.fixture
def dynamics(moon: Moon) -> ThirdBodyGravity:
    return ThirdBodyGravity(moon)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )
    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestThirdBodyGravity:
    def test_constructors(
        self,
        dynamics: ThirdBodyGravity,
    ):
        assert dynamics is not None
        assert isinstance(dynamics, ThirdBodyGravity)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(
        self,
        dynamics: ThirdBodyGravity,
        moon: Moon,
    ):
        assert dynamics.get_celestial() == moon

    def test_apply_contribution(
        self,
        dynamics: ThirdBodyGravity,
        state: State,
    ):
        dxdt: np.ndarray = np.zeros(6)
        dynamics.apply_contribution(state.get_coordinates(), dxdt, state.get_instant())
        assert True
