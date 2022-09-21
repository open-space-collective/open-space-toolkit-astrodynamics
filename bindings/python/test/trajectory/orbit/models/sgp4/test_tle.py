################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/sgp4/test_tle.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

from datetime import datetime

from ostk.core.types import String, Real, Integer

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Derived = physics.units.Derived
Time = physics.units.Time
Angle = physics.units.Angle
Environment = physics.Environment
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Instant = physics.time.Instant
Orbit = astrodynamics.trajectory.Orbit
SGP4 = astrodynamics.trajectory.orbit.models.SGP4
TLE = astrodynamics.trajectory.orbit.models.sgp4.TLE

################################################################################################################################################################

@pytest.fixture
def tle () -> TLE:

    tle: TLE = TLE('1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994','2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316')

    return tle

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_constructors ():

    # Construct Two-Line Element set
    tle: TLE = TLE(
    '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
    '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'
    )

    assert tle is not None
    assert isinstance(tle, TLE)

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_comparators ():

    # Arbitrary TLE
    tle_1: TLE = TLE(
    '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
    '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'
    )

    assert tle_1 == tle_1
    assert (tle_1 != tle_1) is False

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_is_defined (tle: TLE):

    assert tle is not None
    assert tle.is_defined()

    tle: TLE = TLE.undefined()

    assert tle is not None
    assert isinstance(tle, TLE)
    assert tle.is_defined() is False

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_lines (tle: TLE):

    # get_first_line
    first_line: String = tle.get_first_line()

    assert first_line is not None
    assert isinstance(first_line, String)
    assert first_line == '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994'

    # get_second_line
    second_line: String = tle.get_second_line()

    assert second_line is not None
    assert isinstance(second_line, String)
    assert second_line == '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_satellite_number (tle: TLE):

    satellite_number = tle.get_satellite_number()

    assert satellite_number is not None
    assert isinstance(satellite_number, Integer)
    assert satellite_number == 25544

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_classification (tle: TLE):

    # get_classification
    classification: String = tle.get_classification()

    assert classification is not None
    assert isinstance(classification, String)
    assert classification == 'U'

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_international_designator (tle: TLE):

    # get_international_designator
    international_designator: String = tle.get_international_designator()

    assert international_designator is not None
    assert isinstance(international_designator, String)
    assert international_designator == '98067A'

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_epoch (tle: TLE):

    # get_epoch
    epoch: Instant = tle.get_epoch()

    assert epoch is not None
    assert isinstance(epoch, Instant)
    assert epoch == Instant.date_time(DateTime(2018, 8, 19, 4, 17, 27, 231, 360, 0), Scale.UTC)

    date_time : datetime = epoch.get_date_time(Scale.UTC)

    assert date_time is not None
    assert isinstance(date_time, datetime)

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_mean_motion_derivatives (tle: TLE):

    # get_mean_motion_first_time_derivative_divided_by_two
    first_derivative: Real = tle.get_mean_motion_first_time_derivative_divided_by_two()

    assert first_derivative is not None
    assert isinstance(first_derivative, Real)
    assert first_derivative == 1.8700000000000001e-06

    # get_mean_motion_second_time_derivative_divided_by_six
    second_derivative: Real = tle.get_mean_motion_second_time_derivative_divided_by_six()

    assert second_derivative is not None
    assert isinstance(second_derivative, Real)
    assert second_derivative == 0.0

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_b_star_drag_term (tle: TLE):

    # get_b_star_drag_term
    b_star: Real = tle.get_b_star_drag_term()

    assert b_star is not None
    assert isinstance(b_star, Real)
    assert b_star == 1.0196e-05

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_ephemeris_type (tle: TLE):

    ephemeris_type = tle.get_ephemeris_type()

    assert ephemeris_type is not None
    assert isinstance(ephemeris_type, Integer)
    assert ephemeris_type == 0

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_element_set_number (tle: TLE):

    element_set_number = tle.get_element_set_number()

    assert element_set_number is not None
    assert isinstance(element_set_number, Integer)
    assert element_set_number == 999

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_first_line_checksum (tle: TLE):

    checksum = tle.get_first_line_checksum()

    assert checksum is not None
    assert isinstance(checksum, Integer)
    assert checksum == 4

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_inclination (tle: TLE):

    inclination = tle.get_inclination().in_degrees()

    assert inclination is not None
    assert isinstance(inclination, Real)
    assert inclination == 51.6447

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_raan (tle: TLE):

    raan = tle.get_raan().in_degrees()

    assert raan is not None
    assert isinstance(raan, Real)
    assert raan == 64.782399999999996

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_eccentricity (tle: TLE):

    eccentricity = tle.get_eccentricity()

    assert eccentricity is not None
    assert isinstance(eccentricity, Real)
    assert eccentricity == 0.00059710000000000004

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_aop (tle: TLE):

    aop = tle.get_aop().in_degrees()

    assert aop is not None
    assert isinstance(aop, Real)
    assert aop == 73.146699999999996

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_mean_anomaly (tle: TLE):

    mean_anomaly = tle.get_mean_anomaly().in_degrees()

    assert mean_anomaly is not None
    assert isinstance(mean_anomaly, Real)
    assert mean_anomaly == 36.436599999999999

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_mean_motion (tle: TLE):

    mean_motion = tle.get_mean_motion().in_unit(Derived.Unit.angular_velocity(Angle.Unit.Revolution, Time.Unit.Day))

    assert mean_motion is not None
    assert isinstance(mean_motion, Real)
    assert mean_motion == 15.53848234 # Revolutions per Day

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_revolution_number_at_epoch (tle: TLE):

    revolution_at_epoch = tle.get_revolution_number_at_epoch()

    assert revolution_at_epoch is not None
    assert isinstance(revolution_at_epoch, Integer)
    assert revolution_at_epoch == 12831

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_second_line_checksum (tle: TLE):

    checksum = tle.get_second_line_checksum()

    assert checksum is not None
    assert isinstance(checksum, Integer)
    assert checksum == 6

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_can_parse (tle: TLE):

    first_line = tle.get_first_line()
    second_line = tle.get_second_line()

    assert TLE.can_parse(first_line, second_line)

################################################################################################################################################################

# def test_trajectory_orbit_models_sgp4_tle_parse (tle: TLE):

#     ...

################################################################################################################################################################

# def test_trajectory_orbit_models_sgp4_tle_load (tle: TLE):

#     ...

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_generate_checksum (tle: TLE):

    assert TLE.generate_checksum(tle.get_first_line()) == tle.get_first_line_checksum()
    assert TLE.generate_checksum(tle.get_second_line()) == tle.get_second_line_checksum()

    assert TLE.generate_checksum(f'{str(tle.get_first_line())[:-1]}0') == tle.get_first_line_checksum()
    assert TLE.generate_checksum(f'{str(tle.get_second_line())[:-1]}9') == tle.get_second_line_checksum()

################################################################################################################################################################
