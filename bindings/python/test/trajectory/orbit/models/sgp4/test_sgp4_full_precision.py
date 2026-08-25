# Apache License 2.0

import numpy as np
import pytest
from ostk.astrodynamics.trajectory import Orbit, State
from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE, SGP4FullPrecision
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Duration
from ostk.physics.unit import Angle, Derived


@pytest.fixture
def tle() -> TLE:
    return TLE(
        first_line="1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        second_line="2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )


@pytest.fixture
def sgp4_full_precision(tle: TLE) -> SGP4FullPrecision:
    return SGP4FullPrecision.from_tle(tle)


class TestSGP4FullPrecision:
    def test_constructor_success(self, tle: TLE):
        assert (
            SGP4FullPrecision(
                epoch=tle.get_epoch(),
                inclination=tle.get_inclination(),
                raan=tle.get_raan(),
                eccentricity=tle.get_eccentricity(),
                aop=tle.get_aop(),
                mean_anomaly=tle.get_mean_anomaly(),
                mean_motion=tle.get_mean_motion(),
                b_star_drag_term=tle.get_b_star_drag_term(),
                revolution_number_at_epoch=12345,
                output_frame=Frame.GCRF(),
            )
            is not None
        )

    def test_constructor_defaults(self, tle: TLE):
        sgp4_full_precision = SGP4FullPrecision(
            epoch=tle.get_epoch(),
            inclination=tle.get_inclination(),
            raan=tle.get_raan(),
            eccentricity=tle.get_eccentricity(),
            aop=tle.get_aop(),
            mean_anomaly=tle.get_mean_anomaly(),
            mean_motion=tle.get_mean_motion(),
            b_star_drag_term=tle.get_b_star_drag_term(),
        )

        assert sgp4_full_precision.get_revolution_number_at_epoch() == 1
        assert sgp4_full_precision.get_output_frame() == Frame.TEME()

    def test_comparators(self, tle: TLE, sgp4_full_precision: SGP4FullPrecision):
        assert sgp4_full_precision == SGP4FullPrecision.from_tle(tle)
        assert (sgp4_full_precision != SGP4FullPrecision.from_tle(tle)) is False
        assert sgp4_full_precision != SGP4FullPrecision.from_tle(tle, Frame.GCRF())

    def test_is_defined(self, sgp4_full_precision: SGP4FullPrecision):
        assert sgp4_full_precision.is_defined() is True
        assert SGP4FullPrecision.undefined().is_defined() is False

    def test_getters(self, tle: TLE, sgp4_full_precision: SGP4FullPrecision):
        assert sgp4_full_precision.get_epoch() == tle.get_epoch()
        assert isinstance(sgp4_full_precision.get_inclination(), Angle)
        assert isinstance(sgp4_full_precision.get_raan(), Angle)
        assert sgp4_full_precision.get_eccentricity() == tle.get_eccentricity()
        assert isinstance(sgp4_full_precision.get_aop(), Angle)
        assert isinstance(sgp4_full_precision.get_mean_anomaly(), Angle)
        assert isinstance(sgp4_full_precision.get_mean_motion(), Derived)
        assert sgp4_full_precision.get_b_star_drag_term() == tle.get_b_star_drag_term()
        assert (
            sgp4_full_precision.get_revolution_number_at_epoch()
            == tle.get_revolution_number_at_epoch()
        )
        assert sgp4_full_precision.get_output_frame() == Frame.TEME()

    def test_str_repr(self, sgp4_full_precision: SGP4FullPrecision):
        assert str(sgp4_full_precision) is not None
        assert repr(sgp4_full_precision) is not None

    def test_from_tle(self, tle: TLE):
        assert isinstance(SGP4FullPrecision.from_tle(tle), SGP4FullPrecision)
        assert (
            SGP4FullPrecision.from_tle(tle, output_frame=Frame.GCRF()).get_output_frame()
            == Frame.GCRF()
        )

    def test_calculate_state_at(self, tle: TLE, sgp4_full_precision: SGP4FullPrecision):
            SGP4FullPrecision.from_tle(
                tle, output_frame=Frame.GCRF()
            ).get_output_frame()
            == Frame.GCRF()
        )

    def test_calculate_state_at(self, tle: TLE, sgp4_full_precision: SGP4FullPrecision):
        state = sgp4_full_precision.calculate_state_at(tle.get_epoch())

        assert isinstance(state, State)
        assert state.is_defined()
        assert state.get_instant() == tle.get_epoch()
        assert state.get_frame() == Frame.TEME()

        # the output frame is the one the element set was built with
        assert (
            SGP4FullPrecision.from_tle(tle, Frame.GCRF())
            .calculate_state_at(tle.get_epoch())
            .get_frame()
            == Frame.GCRF()
        )

    def test_calculate_states_at(
        self, tle: TLE, sgp4_full_precision: SGP4FullPrecision
    ):
        states = sgp4_full_precision.calculate_states_at(instants)

        assert len(states) == len(instants)
        assert all(isinstance(state, State) for state in states)

    def test_calculate_revolution_number_at(
        self, tle: TLE, sgp4_full_precision: SGP4FullPrecision
    ):
        assert (
            sgp4_full_precision.calculate_revolution_number_at(tle.get_epoch())
            == tle.get_revolution_number_at_epoch()
        )

    def test_usable_as_an_orbit_model(self, sgp4_full_precision: SGP4FullPrecision):
        orbit = Orbit(sgp4_full_precision, Earth.spherical())

        assert orbit.is_defined()
