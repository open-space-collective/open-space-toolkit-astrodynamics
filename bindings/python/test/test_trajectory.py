# Apache License 2.0

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

RealInterval = mathematics.object.RealInterval
Quaternion = mathematics.geometry.d3.transformation.rotation.Quaternion
Length = physics.unit.Length
Angle = physics.unit.Angle
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Duration = physics.time.Duration
Instant = physics.time.Instant
Transform = physics.coordinate.Transform
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.provider.Dynamic
Environment = physics.Environment
Earth = physics.environment.object.celestial.Earth
Trajectory = astrodynamics.Trajectory
Profile = astrodynamics.flight.Profile
SatelliteSystem = astrodynamics.flight.system.SatelliteSystem
Orbit = astrodynamics.trajectory.Orbit
State = astrodynamics.trajectory.State
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.model.Kepler
COE = astrodynamics.trajectory.orbit.model.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.model.sgp4
Access = astrodynamics.Access


def test_trajectory_undefined():
    trajectory: Trajectory = Trajectory.undefined()

    assert trajectory is not None
    assert isinstance(trajectory, Trajectory)
    assert trajectory.is_defined() is False
