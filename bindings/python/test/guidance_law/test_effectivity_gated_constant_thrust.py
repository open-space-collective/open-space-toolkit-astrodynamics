# Apache License 2.0

import pytest

import numpy as np

from ostk.core.type import Real

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant
from ostk.physics.unit import Angle, Derived, Length

from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.guidance_law import EffectivityGatedConstantThrust, QLaw
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Derived(398600.49 * 1e9, Derived.Unit.meter_cubed_per_second_squared())


@pytest.fixture
def target_coe() -> COE:
    return COE(
        Length.meters(42000.0e3),
        Real(0.01),
        Angle.degrees(Real(0.05)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
    )


@pytest.fixture
def current_coe() -> COE:
    return COE(
        Length.meters(7000.0e3),
        Real(0.01),
        Angle.degrees(Real(0.05)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
    )


@pytest.fixture
def q_law(target_coe: COE, gravitational_parameter: Derived) -> QLaw:
    parameters = QLaw.Parameters(
        {
            COE.Element.SemiMajorAxis: (1.0, 100.0),
            COE.Element.Eccentricity: (1.0, 1e-4),
            COE.Element.Inclination: (1.0, 1e-4),
            COE.Element.Raan: (1.0, 1e-4),
            COE.Element.Aop: (1.0, 1e-4),
        }
    )

    return QLaw(
        target_coe,
        gravitational_parameter,
        parameters,
        QLaw.COEDomain.Osculating,
        QLaw.GradientStrategy.Analytical,
    )


@pytest.fixture
def thrust_direction_theta_r_h() -> np.ndarray:
    return np.array([1.0, 0.0, 0.0])


@pytest.fixture
def guidance_law(
    thrust_direction_theta_r_h: np.ndarray, q_law: QLaw
) -> EffectivityGatedConstantThrust:
    return EffectivityGatedConstantThrust(thrust_direction_theta_r_h, q_law)


class TestEffectivityGatedConstantThrust:
    def test_constructor(self, guidance_law: EffectivityGatedConstantThrust):
        assert guidance_law is not None
        assert isinstance(guidance_law, EffectivityGatedConstantThrust)
        assert isinstance(guidance_law, GuidanceLaw)

    def test_getters(
        self,
        guidance_law: EffectivityGatedConstantThrust,
        q_law: QLaw,
    ):
        direction = guidance_law.get_thrust_direction_theta_r_h()
        assert len(direction) == 3
        assert np.isclose(np.linalg.norm(direction), 1.0)

        assert guidance_law.access_q_law() is not None

    def test_calculate_thrust_acceleration_at(
        self,
        guidance_law: EffectivityGatedConstantThrust,
        current_coe: COE,
        gravitational_parameter: Derived,
    ):
        position, velocity = current_coe.get_cartesian_state(
            gravitational_parameter, Frame.GCRF()
        )

        acceleration = guidance_law.calculate_thrust_acceleration_at(
            instant=Instant.J2000(),
            position_coordinates=position.get_coordinates(),
            velocity_coordinates=velocity.get_coordinates(),
            thrust_acceleration=1.0 / 300.0,
            output_frame=Frame.GCRF(),
        )

        assert len(acceleration) == 3
