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
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import CentralBodyGravity


@pytest.fixture
def earth() -> Earth:
    return Earth.spherical()


@pytest.fixture
def dynamics(earth: Earth) -> CentralBodyGravity:
    return CentralBodyGravity(earth)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7000000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([0.0, 0.0, 0.0], frame)
    instant = Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestCentralBodyGravity:
    def test_constructors(
        self,
        dynamics: CentralBodyGravity,
    ):
        assert dynamics is not None
        assert isinstance(dynamics, CentralBodyGravity)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: CentralBodyGravity, earth: Earth):
        assert dynamics.get_celestial() == earth

    def test_compute_contribution(self, dynamics: CentralBodyGravity, state: State):
        contribution = dynamics.compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx([-8.134702887755102, 0.0, 0.0])
