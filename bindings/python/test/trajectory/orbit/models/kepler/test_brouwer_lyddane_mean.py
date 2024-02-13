# Apache License 2.0

import pytest

from ostk.physics.unit import Length
from ostk.physics.unit import Angle

from ostk.astrodynamics.trajectory.orbit.model import BrouwerLyddaneMean


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
def brouwer_lyddane_mean(
    semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
):
    class BrouwerLyddaneMeanMock(BrouwerLyddaneMean):
        def __init__(
            self, semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
        ):
            super().__init__(
                semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
            )

        def to_coe():
            return 0.0

    return BrouwerLyddaneMeanMock(
        semi_major_axis, eccentricity, inclination, raan, aop, mean_anomaly
    )


class TestBrouwerLyddaneMean:
    def test_getters(self, brouwer_lyddane_mean: BrouwerLyddaneMean):
        assert isinstance(brouwer_lyddane_mean.get_mean_anomaly(), Angle)
        assert isinstance(brouwer_lyddane_mean.get_true_anomaly(), Angle)
        assert isinstance(brouwer_lyddane_mean.get_eccentric_anomaly(), Angle)
