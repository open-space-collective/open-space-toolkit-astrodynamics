# Apache License 2.0

import numpy as np
import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Duration
from ostk.physics.unit import Angle
from ostk.physics.unit import Derived

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import MeanElements
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE


@pytest.fixture
def tle() -> TLE:
    return TLE(
        first_line="1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        second_line="2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )


@pytest.fixture
def mean_elements(tle: TLE) -> MeanElements:
    return MeanElements.from_tle(tle)


class TestMeanElements:
    def test_constructor_success(self, tle: TLE):
        assert (
            MeanElements(
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
        mean_elements = MeanElements(
            epoch=tle.get_epoch(),
            inclination=tle.get_inclination(),
            raan=tle.get_raan(),
            eccentricity=tle.get_eccentricity(),
            aop=tle.get_aop(),
            mean_anomaly=tle.get_mean_anomaly(),
            mean_motion=tle.get_mean_motion(),
            b_star_drag_term=tle.get_b_star_drag_term(),
        )

        assert mean_elements.get_revolution_number_at_epoch() == 1
        assert mean_elements.get_output_frame() == Frame.TEME()

    def test_comparators(self, tle: TLE, mean_elements: MeanElements):
        assert mean_elements == MeanElements.from_tle(tle)
        assert (mean_elements != MeanElements.from_tle(tle)) is False
        assert mean_elements != MeanElements.from_tle(tle, Frame.GCRF())

    def test_is_defined(self, mean_elements: MeanElements):
        assert mean_elements.is_defined() is True
        assert MeanElements.undefined().is_defined() is False

    def test_getters(self, tle: TLE, mean_elements: MeanElements):
        assert mean_elements.get_epoch() == tle.get_epoch()
        assert isinstance(mean_elements.get_inclination(), Angle)
        assert isinstance(mean_elements.get_raan(), Angle)
        assert mean_elements.get_eccentricity() == tle.get_eccentricity()
        assert isinstance(mean_elements.get_aop(), Angle)
        assert isinstance(mean_elements.get_mean_anomaly(), Angle)
        assert isinstance(mean_elements.get_mean_motion(), Derived)
        assert mean_elements.get_b_star_drag_term() == tle.get_b_star_drag_term()
        assert (
            mean_elements.get_revolution_number_at_epoch()
            == tle.get_revolution_number_at_epoch()
        )
        assert mean_elements.get_output_frame() == Frame.TEME()

    def test_str_repr(self, mean_elements: MeanElements):
        assert str(mean_elements) is not None
        assert repr(mean_elements) is not None

    def test_from_tle(self, tle: TLE):
        assert isinstance(MeanElements.from_tle(tle), MeanElements)
        assert (
            MeanElements.from_tle(tle, output_frame=Frame.GCRF()).get_output_frame()
            == Frame.GCRF()
        )

    def test_calculate_state_at(self, tle: TLE, mean_elements: MeanElements):
        state = mean_elements.calculate_state_at(tle.get_epoch())

        assert isinstance(state, State)
        assert state.is_defined()
        assert state.get_instant() == tle.get_epoch()
        assert state.get_frame() == Frame.TEME()

        # the output frame is the one the element set was built with
        assert (
            MeanElements.from_tle(tle, Frame.GCRF())
            .calculate_state_at(tle.get_epoch())
            .get_frame()
            == Frame.GCRF()
        )

    def test_calculate_states_at(self, tle: TLE, mean_elements: MeanElements):
        instants = [
            tle.get_epoch(),
            tle.get_epoch() + Duration.minutes(10.0),
        ]

        states = mean_elements.calculate_states_at(instants)

        assert len(states) == len(instants)
        assert all(isinstance(state, State) for state in states)

    def test_calculate_revolution_number_at(self, tle: TLE, mean_elements: MeanElements):
        assert (
            mean_elements.calculate_revolution_number_at(tle.get_epoch())
            == tle.get_revolution_number_at_epoch()
        )

    def test_usable_as_an_orbit_model(self, mean_elements: MeanElements):
        orbit = Orbit(mean_elements, Earth.spherical())

        assert orbit.is_defined()

    def test_matches_sgp4_from_tle(self, tle: TLE, mean_elements: MeanElements):
        """Fed the values a TLE carries, it must agree with the TLE path.

        SGP4 decodes its TLE into a MeanElements and propagates that, so the two are the
        same computation. MeanElements.test.cpp is where this is actually pinned down.
        """
        sgp4 = SGP4(tle, Frame.TEME())
        instant = tle.get_epoch() + Duration.minutes(60.0)

        from_mean_elements = mean_elements.calculate_state_at(instant)
        from_tle = sgp4.calculate_state_at(instant)

        assert (
            np.linalg.norm(
                from_mean_elements.get_position().get_coordinates()
                - from_tle.get_position().get_coordinates()
            )
            < 1e-12
        )
