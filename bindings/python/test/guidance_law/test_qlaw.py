# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.unit import Derived
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.time import Instant
from ostk.physics.unit import Length
from ostk.physics.unit import Angle


from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.guidance_law import QLaw
from ostk.astrodynamics.trajectory import State


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
        m=5,
        n=6,
        r=7,
        b=0.08,
        k=9,
        periapsis_weight=0.1,
        minimum_periapsis_radius=Length.kilometers(7000.0),
        absolute_effectivity_threshold=0.2,
        relative_effectivity_threshold=0.3,
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
        coe_domain=QLaw.COEDomain.Osculating,
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


@pytest.fixture
def position(frame: Frame, position_coordinates: list[float]) -> Position:
    return Position.meters(position_coordinates, frame)


@pytest.fixture
def velocity(frame: Frame, velocity_coordinates: list[float]) -> Velocity:
    return Velocity.meters_per_second(velocity_coordinates, frame)


@pytest.fixture
def state(instant: Instant, position: Position, velocity: Velocity) -> State:
    return State(instant=instant, position=position, velocity=velocity)


class TestQLawParameters:
    def test_constructors(self, parameters: QLaw.Parameters):
        assert parameters is not None
        assert isinstance(parameters, QLaw.Parameters)

    def test_getters(self, parameters: QLaw.Parameters):
        assert parameters.get_control_weights() is not None
        assert parameters.get_convergence_thresholds() is not None
        assert parameters.m == 5
        assert parameters.n == 6
        assert parameters.r == 7
        assert parameters.b == 0.08
        assert parameters.k == 9
        assert parameters.periapsis_weight == 0.1
        assert parameters.get_minimum_periapsis_radius() == Length.kilometers(7000.0)
        assert parameters.absolute_effectivity_threshold == 0.2
        assert parameters.relative_effectivity_threshold == 0.3

    def test_set_and_get_control_weight(self):
        parameters = QLaw.Parameters(
            element_weights={
                COE.Element.SemiMajorAxis: (0.1, 100.0),
                COE.Element.Eccentricity: (0.2, 1e-3),
                COE.Element.Inclination: (0.3, 1.0),
                COE.Element.Raan: (0.4, 2.0),
                COE.Element.Aop: (0.5, 3.0),
            },
        )

        # Verify initial weights
        assert parameters.get_control_weight(COE.Element.SemiMajorAxis) == 0.1
        assert parameters.get_control_weight(COE.Element.Eccentricity) == 0.2
        assert parameters.get_control_weight(COE.Element.Inclination) == 0.3
        assert parameters.get_control_weight(COE.Element.Raan) == 0.4
        assert parameters.get_control_weight(COE.Element.Aop) == 0.5

        # Update weights
        parameters.set_control_weight(COE.Element.SemiMajorAxis, 1.0)
        parameters.set_control_weight(COE.Element.Eccentricity, 2.0)
        parameters.set_control_weight(COE.Element.Inclination, 3.0)
        parameters.set_control_weight(COE.Element.Raan, 4.0)
        parameters.set_control_weight(COE.Element.Aop, 5.0)

        # Verify updated weights
        assert parameters.get_control_weight(COE.Element.SemiMajorAxis) == 1.0
        assert parameters.get_control_weight(COE.Element.Eccentricity) == 2.0
        assert parameters.get_control_weight(COE.Element.Inclination) == 3.0
        assert parameters.get_control_weight(COE.Element.Raan) == 4.0
        assert parameters.get_control_weight(COE.Element.Aop) == 5.0

        # Verify via get_control_weights as well
        control_weights = parameters.get_control_weights()
        assert control_weights[0] == 1.0
        assert control_weights[1] == 2.0
        assert control_weights[2] == 3.0
        assert control_weights[3] == 4.0
        assert control_weights[4] == 5.0

        # Test invalid element throws
        with pytest.raises(RuntimeError):
            parameters.set_control_weight(COE.Element.TrueAnomaly, 1.0)
        with pytest.raises(RuntimeError):
            parameters.get_control_weight(COE.Element.TrueAnomaly)


class TestQLaw:
    def test_constructors(
        self,
        target_COE: COE,
        gravitational_parameter: Derived,
        parameters: QLaw.Parameters,
        gradient_strategy: QLaw.GradientStrategy,
    ):
        qlaw: QLaw = QLaw(
            target_coe=target_COE,
            gravitational_parameter=gravitational_parameter,
            parameters=parameters,
            coe_domain=QLaw.COEDomain.BrouwerLyddaneMeanLong,
            gradient_strategy=gradient_strategy,
        )

        assert qlaw is not None
        assert isinstance(qlaw, QLaw)
        assert isinstance(qlaw, GuidanceLaw)

    def test_getters(self, q_law: QLaw):
        assert q_law.get_parameters() is not None
        assert q_law.get_target_coe() is not None
        assert q_law.get_gradient_strategy() is not None
        assert q_law.get_coe_domain() is not None

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

    def test_compute_effectivity(
        self,
        q_law: QLaw,
        state: State,
        thrust_acceleration: float,
    ):
        assert (
            q_law.compute_effectivity(
                state=state,
                thrust_acceleration=thrust_acceleration,
            )
            is not None
        )

        assert (
            q_law.compute_effectivity(
                state=state,
                thrust_acceleration=thrust_acceleration,
                discretization_step_count=15,
            )
            is not None
        )
