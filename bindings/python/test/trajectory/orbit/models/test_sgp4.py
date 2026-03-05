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


class TestSGP4:
    def test_constructor_default(self, tle: TLE):
        sgp4 = SGP4(tle)
        assert sgp4.is_defined()
        assert sgp4.get_output_frame() == Frame.GCRF()

    def test_constructor_with_output_frame(self, tle: TLE):
        sgp4_teme = SGP4(tle, Frame.TEME())
        assert sgp4_teme.is_defined()
        assert sgp4_teme.get_output_frame() == Frame.TEME()

        sgp4_gcrf = SGP4(tle, Frame.GCRF())
        assert sgp4_gcrf.is_defined()
        assert sgp4_gcrf.get_output_frame() == Frame.GCRF()

    def test_calculate_state_at_output_frame(self, tle: TLE):
        sgp4_default = SGP4(tle)
        sgp4_teme = SGP4(tle, Frame.TEME())

        instant = tle.get_epoch()

        state_default = sgp4_default.calculate_state_at(instant)
        state_teme = sgp4_teme.calculate_state_at(instant)

        # Default outputs GCRF
        assert state_default.get_frame() == Frame.GCRF()

        # TEME constructor outputs TEME
        assert state_teme.get_frame() == Frame.TEME()

        # Both should give same position when converted to same frame
        state_teme_in_gcrf = state_teme.in_frame(Frame.GCRF())
        pos_diff = (
            state_default.get_position().get_coordinates()
            - state_teme_in_gcrf.get_position().get_coordinates()
        )
        assert pos_diff.norm() < 1e-6
