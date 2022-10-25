################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/test_orbit.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Length = physics.units.Length
Angle = physics.units.Angle
Scale = physics.time.Scale
Instant = physics.time.Instant
Interval = physics.time.Interval
DateTime = physics.time.DateTime
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Environment = physics.Environment

Trajectory = astrodynamics.Trajectory
Model = astrodynamics.trajectory.Model
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.SGP4
TLE = astrodynamics.trajectory.orbit.models.sgp4.TLE
State = astrodynamics.trajectory.State
Access = astrodynamics.Access

earth = Environment.default().access_celestial_object_with_name('Earth')

################################################################################################################################################################

def test_trajectory_orbit_constructors ():

    # Construct Two-Line Element set
    tle = TLE('1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994','2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316')

    # Construct orbit using SGP4 model
    orbit = Orbit(SGP4(tle), earth)

    assert orbit is not None
    assert isinstance(orbit, Orbit)
    assert orbit.is_defined()

    # Construct get state at current epoch
    state: State = orbit.get_state_at(Instant.now())

    assert state is not None
    assert isinstance(state, State)

################################################################################################################################################################

def test_trajectory_orbit_circular ():

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    altitude = Length.kilometers(500.0)
    inclination = Angle.degrees(45.0)

    orbit: Orbit = Orbit.circular(epoch, altitude, inclination, earth)

################################################################################################################################################################

@pytest.mark.skip
def test_trajectory_orbit_equatorial ():

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    altitude = Length.kilometers(500.0)
    eccentricity = 0.1

    orbit: Orbit = Orbit.equatorial(epoch, altitude, eccentricity, earth)

    assert orbit is not None
    assert isinstance(orbit, Orbit)
    assert orbit.is_defined()

################################################################################################################################################################

@pytest.mark.skip
def test_trajectory_orbit_circular_equatorial ():

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    altitude = Length.kilometers(500.0)

    orbit: Orbit = Orbit.circular_equatorial(epoch, altitude, earth)

    assert orbit is not None
    assert isinstance(orbit, Orbit)
    assert orbit.is_defined()

################################################################################################################################################################

@pytest.mark.skip
def test_trajectory_orbit_sun_synchronous ():

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    altitude = Length.kilometers(500.0)

    orbit: Orbit = Orbit.sun_synchronous(epoch, altitude, earth)

    assert orbit is not None
    assert isinstance(orbit, Orbit)
    assert orbit.is_defined()

################################################################################################################################################################
