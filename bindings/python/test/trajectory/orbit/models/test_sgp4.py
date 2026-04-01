# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.time import Duration
from ostk.physics.time import Interval

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


@pytest.fixture
def sgp4_multi(tle: TLE, tle2: TLE) -> SGP4:
    return SGP4([tle, tle2])


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

    def test_constructor_tles(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2])
        assert sgp4 is not None

        assert SGP4([tle, tle2], Frame.GCRF()) is not None

    def test_constructor_tles_with_intervals(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        boundary = tle.get_epoch() + Duration.hours(18.0)
        interval1 = Interval.half_open_right(
            tle.get_epoch() - Duration.days(1.0), boundary
        )
        interval2 = Interval.closed(boundary, tle2.get_epoch() + Duration.days(1.0))

        sgp4 = SGP4([(tle, interval1), (tle2, interval2)])
        assert sgp4 is not None

        assert SGP4([(tle, interval1), (tle2, interval2)], Frame.GCRF()) is not None

    def test_get_validity_intervals(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        sgp4 = SGP4([tle, tle2])
        intervals = sgp4.get_validity_intervals()
        assert intervals is not None
        assert len(intervals) == 2

    def test_get_validity_intervals_single_tle(
        self,
        sgp4: SGP4,
    ):
        intervals = sgp4.get_validity_intervals()
        # Single TLE constructor doesn't generate intervals
        assert len(intervals) == 0

    def test_calculate_state_at_multi_tle_custom_intervals(
        self,
        tle: TLE,
        tle2: TLE,
    ):
        # Set boundary at 18 hours (not midpoint at 12 hours)
        boundary = tle.get_epoch() + Duration.hours(18.0)
        interval1 = Interval.half_open_right(
            tle.get_epoch() - Duration.days(100.0), boundary
        )
        interval2 = Interval.closed(boundary, tle2.get_epoch() + Duration.days(100.0))

        sgp4 = SGP4([(tle, interval1), (tle2, interval2)])

        state = sgp4.calculate_state_at(tle.get_epoch() + Duration.hours(15.0))
        assert state is not None

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
        assert len(tles) == 1

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

    @pytest.mark.parametrize(
        "fixture_name",
        ["sgp4", "sgp4_multi"],
    )
    def test_calculate_state_at(
        self,
        fixture_name: str,
        tle: TLE,
        request,
    ):
        sgp4 = request.getfixturevalue(fixture_name)
        assert sgp4.calculate_state_at(tle.get_epoch()) is not None

    @pytest.mark.parametrize(
        "fixture_name",
        ["sgp4", "sgp4_multi"],
    )
    def test_calculate_states_at(
        self,
        fixture_name: str,
        tle: TLE,
        request,
    ):
        sgp4 = request.getfixturevalue(fixture_name)
        instants = [
            tle.get_epoch(),
            tle.get_epoch() + Duration.hours(1.0),
            tle.get_epoch() + Duration.hours(2.0),
        ]
        states = sgp4.calculate_states_at(instants)
        assert states is not None
        assert len(states) == 3
