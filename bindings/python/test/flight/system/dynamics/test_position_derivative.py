# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import Dynamics
from ostk.astrodynamics.flight.system.dynamics import PositionDerivative


@pytest.fixture
def dynamics() -> PositionDerivative:
    return PositionDerivative()


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )
    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestPositionDerivative:
    def test_constructors(
        self,
        dynamics: PositionDerivative,
    ):
        assert dynamics is not None
        assert isinstance(dynamics, PositionDerivative)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_get_contribution(self, dynamics: PositionDerivative, state: State):
        dxdt: np.ndarray = np.zeros(6)
        dynamics.get_contribution(state.get_coordinates(), dxdt, state.get_instant())
        assert True
