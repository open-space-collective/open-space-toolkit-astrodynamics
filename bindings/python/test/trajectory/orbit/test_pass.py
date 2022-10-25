################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/test_pass.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

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

earth = Environment.default().access_celestial_object_with_name("Earth")

################################################################################################################################################################

def test_trajectory_orbit_pass ():

    pass_type = Pass.Type.Partial
    pass_revolution_number = 123
    pass_start_instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    pass_end_instant = Instant.date_time(DateTime(2018, 1, 1, 1, 0, 0), Scale.UTC)
    pass_interval = Interval.closed(pass_start_instant, pass_end_instant)

    pass_ = Pass(pass_type, pass_revolution_number, pass_interval)

    assert pass_ is not None
    assert isinstance(pass_, Pass)

    assert pass_.is_defined()
    assert pass_.is_complete() is not None
    assert pass_.get_type() is not None
    assert pass_.get_revolution_number() is not None
    # Interval conversion to Python type of issue
    # assert pass_.get_interval() is not None

    assert Pass.string_from_type(Pass.Type.Complete) is not None
    assert Pass.string_from_phase(Pass.Phase.Ascending) is not None
    assert Pass.string_from_quarter(Pass.Quarter.First) is not None

################################################################################################################################################################
