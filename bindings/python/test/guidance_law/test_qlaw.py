# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.physics.unit import Derived
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.physics.unit import Length
from ostk.physics.unit import Angle


from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.guidance_law import QLaw


@pytest.fixture
def target_COE() -> COE:
    return COE(
        Length.meters(42000.0e3),
        0.01,
        Angle.degrees(0.05),
        Angle.degrees(0.0),
        Angle.degrees(0.0),
        Angle.degrees(0.0),
    )


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Derived(
        3.986004418e14,
        EarthGravitationalModel.EGM2008.gravitational_parameter.get_unit(),
    )


@pytest.fixture
def parameters() -> QLaw.Parameters:
    return QLaw.Parameters(
        element_weights={
            COE.Element.SemiMajorAxis: (1.0, 100.0),
            COE.Element.Eccentricity: (1.0, 1e-3),
        },
    )


@pytest.fixture
def gradient_strategy() -> QLaw.GradientStrategy:
    return QLaw.GradientStrategy.FiniteDifference


@pytest.fixture
def q_law(
    target_COE: COE,
    gravitational_parameter: Derived,
    parameters: QLaw.Parameters,
    gradient_strategy: QLaw.GradientStrategy,
) -> QLaw:
    return QLaw(
        target_coe=target_COE,
        gravitational_parameter=gravitational_parameter,
        parameters=parameters,
        gradient_strategy=gradient_strategy,
    )


@pytest.fixture
def thrust_acceleration() -> float:
    return 1.0 / 300.0


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def position_coordinates() -> list[float]:
    return [6930000.0, 0.0, 0.0]


@pytest.fixture
def velocity_coordinates() -> list[float]:
    return [0.0, 7621.89248591193, 6.65135764404186]


@pytest.fixture
def instant() -> Instant:
    return Instant.J2000()


class TestQLawParameters:
    def test_constructors(self, parameters: QLaw.Parameters):
        assert parameters is not None
        assert isinstance(parameters, QLaw.Parameters)

    def test_getters(self, parameters: QLaw.Parameters):
        assert parameters.get_control_weights() is not None
        assert parameters.m is not None
        assert parameters.n is not None
        assert parameters.r is not None
        assert parameters.b is not None


class TestQLaw:
    def test_constructors(self, q_law: QLaw):
        assert q_law is not None
        assert isinstance(q_law, QLaw)
        assert isinstance(q_law, GuidanceLaw)

    def test_getters(self, q_law: QLaw):
        assert q_law.get_parameters() is not None
        assert q_law.get_target_coe() is not None
        assert q_law.get_gradient_strategy() is not None

    def test_calculate_thrust_acceleration_at(
        self,
        q_law: QLaw,
        position_coordinates: list[float],
        velocity_coordinates: list[float],
        thrust_acceleration: float,
        instant: Instant,
        frame: Frame,
    ):
        assert pytest.approx(
            q_law.calculate_thrust_acceleration_at(
                instant=instant,
                position_coordinates=position_coordinates,
                velocity_coordinates=velocity_coordinates,
                thrust_acceleration=thrust_acceleration,
                output_frame=frame,
            )
        ) == np.array([0.0, 0.0033333320640941645, 2.9088817174504986e-06])
