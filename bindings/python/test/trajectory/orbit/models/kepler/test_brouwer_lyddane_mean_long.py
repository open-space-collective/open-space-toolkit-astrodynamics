# Apache License 2.0

import pytest

from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.environment.gravitational import Earth

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity

from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.trajectory.orbit.model.brouwerLyddaneMean import (
    BrouwerLyddaneMeanLong,
)


@pytest.fixture
def semi_major_axis() -> Length:
    return Length.kilometers(7000.0)


@pytest.fixture
def eccentricity() -> float:
    return 1e-3


@pytest.fixture
def inclination() -> Angle:
    return Angle.degrees(35.0)


@pytest.fixture
def raan() -> Angle:
    return Angle.degrees(40.0)


@pytest.fixture
def aop() -> Angle:
    return Angle.degrees(50.0)


@pytest.fixture
def mean_anomaly() -> Angle:
    return Angle.degrees(60.0)


@pytest.fixture
def coe(
    semi_major_axis: Length,
    eccentricity: float,
    inclination: Angle,
    raan: Angle,
    aop: Angle,
    mean_anomaly: Angle,
) -> COE:
    return COE(semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly)


@pytest.fixture
def cartesian_state() -> tuple[Position, Velocity]:
    return (
        Position.meters(
            [6596407.223662058, 2281266.582975321, -10540.61521486086], Frame.GCRF()
        ),
        Velocity.meters_per_second(
            [337.7269674273224, -969.7192552349448, 7488.702816619139], Frame.GCRF()
        ),
    )


@pytest.fixture
def gravitational_parameter():
    return Earth.EGM2008.gravitational_parameter


@pytest.fixture
def brouwer_lyddane_mean_long(
    semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
):
    return BrouwerLyddaneMeanLong(
        semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
    )


class TestBrouwerLyddaneMeanLong:
    def test_to_coe(self, brouwer_lyddane_mean_long: BrouwerLyddaneMeanLong):
        assert brouwer_lyddane_mean_long.to_coe().is_defined()

    def test_coe(self, coe: COE):
        assert BrouwerLyddaneMeanLong.COE(coe).is_defined()

    def test_cartesian(
        self, cartesian_state: tuple[Position, Velocity], gravitational_parameter
    ):
        assert BrouwerLyddaneMeanLong.cartesian(
            cartesian_state, gravitational_parameter
        ).is_defined()

    def test_undefined(self):
        assert BrouwerLyddaneMeanLong.undefined().is_defined() is False
