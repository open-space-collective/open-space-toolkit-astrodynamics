# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Moon

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import AnalyticalThirdBodyGravity
from ostk.astrodynamics.dynamics import ThirdBodyGravity


@pytest.fixture
def moon() -> Moon:
    return Moon.spherical()


@pytest.fixture
def dynamics(moon: Moon) -> AnalyticalThirdBodyGravity:
    return AnalyticalThirdBodyGravity(moon)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7000000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([0.0, 0.0, 0.0], frame)
    instant = Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestAnalyticalThirdBodyGravity:
    def test_constructors(
        self,
        dynamics: AnalyticalThirdBodyGravity,
        moon: Moon,
    ):
        assert dynamics is not None
        assert isinstance(dynamics, AnalyticalThirdBodyGravity)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

        named_dynamics = AnalyticalThirdBodyGravity(celestial=moon, name="test")

        assert named_dynamics.get_name() == "test"

    def test_getters(
        self,
        dynamics: AnalyticalThirdBodyGravity,
        moon: Moon,
    ):
        assert dynamics.get_celestial() == moon

    def test_compute_contribution(
        self,
        dynamics: AnalyticalThirdBodyGravity,
        moon: Moon,
        state: State,
    ):
        contribution = dynamics.compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert len(contribution) == 3

        # Agrees with the ephemeris-based dynamics to within the accuracy of the analytical position

        ephemeris_contribution = ThirdBodyGravity(moon).compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert contribution == pytest.approx(ephemeris_contribution, rel=1e-2)
