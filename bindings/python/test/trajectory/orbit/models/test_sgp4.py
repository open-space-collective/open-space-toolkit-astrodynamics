# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.time import Duration

from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE


@pytest.fixture
def tle() -> TLE:
    return TLE(
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )


@pytest.fixture
def tle2() -> TLE:
    tle = TLE(
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )
    tle.set_epoch(tle.get_epoch() + Duration.days(1.0))
    return tle


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

    def test_constructor_with_tle_array(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2])
        assert sgp4 is not None
        assert sgp4.is_defined()

    def test_constructor_with_tle_array_and_output_frame(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2], Frame.TEME())
        assert sgp4 is not None
        assert sgp4.is_defined()
        assert sgp4.get_output_frame() == Frame.TEME()

    def test_constructor_with_empty_tle_array(self):
        with pytest.raises(RuntimeError):
            SGP4([])

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

    def test_get_tles(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2])
        tles = sgp4.get_tles()
        assert tles is not None
        assert len(tles) == 2

    def test_get_tles_single_tle(
        self,
        sgp4: SGP4,
    ):
        tles = sgp4.get_tles()
        assert len(tles) == 0

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

    def test_calculate_state_at_multi_tle(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4_multi = SGP4([tle, tle2])
        sgp4_single = SGP4(tle)

        # State at tle epoch should match single-TLE model
        state_multi = sgp4_multi.calculate_state_at(tle.get_epoch())
        state_single = sgp4_single.calculate_state_at(tle.get_epoch())

        assert state_multi is not None
        assert state_single is not None

    def test_calculate_states_at(
        self,
        sgp4: SGP4,
        tle: TLE,
    ):
        instants = [
            tle.get_epoch(),
            tle.get_epoch() + Duration.hours(1.0),
            tle.get_epoch() + Duration.hours(2.0),
        ]
        states = sgp4.calculate_states_at(instants)
        assert states is not None
        assert len(states) == 3

    def test_calculate_states_at_multi_tle(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2])
        instants = [
            tle.get_epoch(),
            tle2.get_epoch(),
        ]
        states = sgp4.calculate_states_at(instants)
        assert states is not None
        assert len(states) == 2
