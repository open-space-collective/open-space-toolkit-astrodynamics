################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/sgp4/test_tlepy
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

from ostk.core.types import String, Real

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Environment = physics.Environment
DateTime = physics.time.DateTime
Instant = physics.time.Instant
Orbit = astrodynamics.trajectory.Orbit
SGP4 = astrodynamics.trajectory.orbit.models.SGP4
TLE = astrodynamics.trajectory.orbit.models.sgp4.TLE

################################################################################################################################################################

def construct_tle ():

    tle: TLE = TLE(
    '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
    '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'
    )

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

    # Construct Three-Line Element set

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

def test_trajectory_orbit_models_sgp4_tle_is_defined ():

    tle: TLE = construct_tle()

    assert tle is not None
    assert tle.is_defined()

    tle: TLE = TLE.undefined()

    assert tle is not None
    assert isinstance(tle, TLE)
    assert tle.is_defined() is False

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_lines ():

    tle: TLE = construct_tle()

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

# def test_trajectory_orbit_models_sgp4_tle_get_sat_number ():

#     tle: TLE = construct_tle()

    # get_satellite_number

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_classification ():

    tle: TLE = construct_tle()

    # get_classification
    classification: String = tle.get_classification()

    assert classification is not None
    assert isinstance(classification, String)
    assert classification == 'U'

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_international_designator ():

    tle: TLE = construct_tle()

    # get_international_designator
    international_designator: String = tle.get_international_designator()

    assert international_designator is not None
    assert isinstance(international_designator, String)
    assert international_designator == '98067A'

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_epoch ():

    tle: TLE = construct_tle()

    # get_epoch
    epoch: Instant = tle.get_epoch()

    assert epoch is not None
    assert isinstance(epoch, Instant)
    # assert epoch == Instant.date_time(DateTime(2018, 8, 19, 4, 17, 27, 231, 360, 0), Scale.UTC)

################################################################################################################################################################

def test_trajectory_orbit_models_sgp4_tle_get_mean_motions ():

    tle: TLE = construct_tle()

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

def test_trajectory_orbit_models_sgp4_tle_get_b_star_drag_term ():

    tle: TLE = construct_tle()

    # get_b_star_drag_term
    b_star: Real = tle.get_b_star_drag_term()

    assert b_star is not None
    assert isinstance(b_star, Real)
    assert b_star == 1.0196e-05

################################################################################################################################################################

# ...

################################################################################################################################################################
