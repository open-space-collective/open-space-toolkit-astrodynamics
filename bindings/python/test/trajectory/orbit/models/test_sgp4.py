# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE


@pytest.fixture
def tle() -> TLE:
    return TLE(
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )


@pytest.fixture
def sgp4(tle: TLE) -> SGP4:
    return SGP4(tle)


class TestSGP4:
    def test_constructor(
        self,
        tle: TLE,
    ):
        assert SGP4(tle) is not None

    def test_constructor_with_output_frame(
        self,
        tle: TLE,
    ):
        assert SGP4(tle, Frame.TEME()) is not None

    def test_is_defined(
        self,
        sgp4: SGP4,
    ):
        assert sgp4.is_defined()

    def test_get_tle(
        self,
        sgp4: SGP4,
    ):
        assert sgp4.get_tle() is not None

    def test_get_output_frame(
        self,
        sgp4: SGP4,
    ):
        assert sgp4.get_output_frame() is not None

    def test_get_epoch(
        self,
        sgp4: SGP4,
    ):
        assert sgp4.get_epoch() is not None

    def test_get_revolution_number_at_epoch(
        self,
        sgp4: SGP4,
    ):
        assert sgp4.get_revolution_number_at_epoch() is not None

    def test_calculate_state_at(
        self,
        sgp4: SGP4,
        tle: TLE,
    ):
        assert sgp4.calculate_state_at(tle.get_epoch()) is not None
