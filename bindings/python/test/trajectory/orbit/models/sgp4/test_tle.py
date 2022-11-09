################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/sgp4/test_tle.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import tempfile

from ostk.core.filesystem import Path
from ostk.core.filesystem import File

from ostk.physics.units import Time
from ostk.physics.units import Angle
from ostk.physics.units import Derived
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.time import DateTime

from ostk.astrodynamics.trajectory.orbit.models.sgp4 import TLE

################################################################################################################################################################

@pytest.fixture
def tle () -> TLE:

    return TLE(
        satellite_name = 'Satellite',
        first_line = '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
        second_line = '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316',
    )

################################################################################################################################################################

class TestTLE:

    def test_constructor (self):

        tle = TLE(
            first_line = '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
            second_line = '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316',
        )

        assert isinstance(tle, TLE)

    def test_constructor_with_satellite_name (self):

        tle = TLE(
            satellite_name = 'Satellite',
            first_line = '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
            second_line = '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316',
        )

        assert isinstance(tle, TLE)

    def test_comparators (self, tle: TLE):

        assert (tle == tle) is True
        assert (tle != tle) is False

    def test_is_defined (self, tle: TLE):

        assert tle.is_defined()

    def test_get_satellite_name (self, tle: TLE):

        assert tle.get_satellite_name() == 'Satellite'

    def test_get_first_line (self, tle: TLE):

        assert tle.get_first_line() == '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994'

    def test_get_second_line (self, tle: TLE):

        assert tle.get_second_line() == '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'

    def test_get_satellite_number (self, tle: TLE):

        assert tle.get_satellite_number() == 25544

    def test_get_classification (self, tle: TLE):

        assert tle.get_classification() == 'U'

    def test_get_international_designator (self, tle: TLE):

        assert tle.get_international_designator() == '98067A'

    def test_get_epoch (self, tle: TLE):

        assert tle.get_epoch() == Instant.date_time(DateTime(2018, 8, 19, 4, 17, 27, 231, 360, 0), Scale.UTC)

    def test_get_mean_motion_first_time_derivative_divided_by_two (self, tle: TLE):

        assert tle.get_mean_motion_first_time_derivative_divided_by_two() == 1.8700000000000001e-06

    def test_get_mean_motion_second_time_derivative_divided_by_six (self, tle: TLE):

        assert tle.get_mean_motion_second_time_derivative_divided_by_six() == 0.0

    def test_get_b_star_drag_term (self, tle: TLE):

        assert tle.get_b_star_drag_term() == 1.0196e-05

    def test_get_ephemeris_type (self, tle: TLE):

        assert tle.get_ephemeris_type() == 0

    def test_get_element_set_number (self, tle: TLE):

        assert tle.get_element_set_number() == 999

    def test_get_first_line_checksum (self, tle: TLE):

        assert tle.get_first_line_checksum() == 4

    def test_get_inclination (self, tle: TLE):

        assert tle.get_inclination().in_degrees() == 51.6447

    def test_get_raan (self, tle: TLE):

        assert tle.get_raan().in_degrees() == 64.782399999999996

    def test_get_eccentricity (self, tle: TLE):

        assert tle.get_eccentricity() == 0.00059710000000000004

    def test_get_aop (self, tle: TLE):

        assert tle.get_aop().in_degrees() == 73.146699999999996

    def test_get_mean_anomaly (self, tle: TLE):

        assert tle.get_mean_anomaly().in_degrees() == 36.436599999999999

    def test_get_mean_motion (self, tle: TLE):

        assert tle.get_mean_motion().in_unit(Derived.Unit.angular_velocity(Angle.Unit.Revolution, Time.Unit.Day)) == 15.53848234

    def test_get_revolution_number_at_epoch (self, tle: TLE):

        assert tle.get_revolution_number_at_epoch() == 12831

    def test_get_second_line_checksum (self, tle: TLE):

        assert tle.get_second_line_checksum() == 6

    def test_can_parse (self, tle: TLE):

        assert TLE.can_parse(tle.get_first_line(), tle.get_second_line())

    def test_parse (self, tle: TLE):

        tle_lines: str = '\n'.join([
            str(tle.get_satellite_name()),
            str(tle.get_first_line()),
            str(tle.get_second_line()),
        ])

        assert TLE.parse(tle_lines) == tle

    def test_load (self, tle: TLE):

        tle_lines: str = '\n'.join([
            str(tle.get_satellite_name()),
            str(tle.get_first_line()),
            str(tle.get_second_line()),
        ])

        tle_file = tempfile.NamedTemporaryFile(suffix = '.tle', delete = False)
        tle_file.write(tle_lines.encode())
        tle_file.close()

        try:
            assert TLE.load(File.path(Path.parse(tle_file.name))) == tle

        finally:
            File.path(Path.parse(tle_file.name)).remove()

    def test_construct_with_satellite_name (self, tle: TLE):

        constructed_tle = TLE.construct(
            satellite_name = 'Satellite',
            satellite_number = 25544,
            classification = 'U',
            international_designator = '98067A',
            epoch = Instant.date_time(DateTime(2018, 8, 19, 4, 17, 27, 231, 360, 0), Scale.UTC),
            mean_motion_first_time_derivative_divided_by_two = 1.8700000000000001e-06,
            mean_motion_second_time_derivative_divided_by_six = 0.0,
            b_star_drag_term = 1.0196e-05,
            ephemeris_type = 0,
            element_set_number = 999,
            inclination = Angle.degrees(51.6447),
            raan = Angle.degrees(64.782399999999996),
            eccentricity = 0.00059710000000000004,
            aop = Angle.degrees(73.146699999999996),
            mean_anomaly = Angle.degrees(36.436599999999999),
            mean_motion = Derived(15.53848234, Derived.Unit.angular_velocity(Angle.Unit.Revolution, Time.Unit.Day)),
            revolution_number_at_epoch = 12831,
        )

        assert constructed_tle == tle

    def test_construct_without_satellite_name (self, tle: TLE):

        constructed_tle = TLE.construct(
            satellite_number = 25544,
            classification = 'U',
            international_designator = '98067A',
            epoch = Instant.date_time(DateTime(2018, 8, 19, 4, 17, 27, 231, 360, 0), Scale.UTC),
            mean_motion_first_time_derivative_divided_by_two = 1.8700000000000001e-06,
            mean_motion_second_time_derivative_divided_by_six = 0.0,
            b_star_drag_term = 1.0196e-05,
            ephemeris_type = 0,
            element_set_number = 999,
            inclination = Angle.degrees(51.6447),
            raan = Angle.degrees(64.782399999999996),
            eccentricity = 0.00059710000000000004,
            aop = Angle.degrees(73.146699999999996),
            mean_anomaly = Angle.degrees(36.436599999999999),
            mean_motion = Derived(15.53848234, Derived.Unit.angular_velocity(Angle.Unit.Revolution, Time.Unit.Day)),
            revolution_number_at_epoch = 12831,
        )

        assert constructed_tle.get_first_line() == tle.get_first_line()
        assert constructed_tle.get_second_line() == tle.get_second_line()

    def test_generate_checksum (self, tle: TLE):

        assert TLE.generate_checksum(tle.get_first_line()) == tle.get_first_line_checksum()
        assert TLE.generate_checksum(tle.get_second_line()) == tle.get_second_line_checksum()

        assert TLE.generate_checksum(f'{str(tle.get_first_line())[:-1]}0') == tle.get_first_line_checksum()
        assert TLE.generate_checksum(f'{str(tle.get_second_line())[:-1]}9') == tle.get_second_line_checksum()

################################################################################################################################################################
