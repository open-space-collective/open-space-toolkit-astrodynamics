################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/test_trajectory.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

RealInterval = mathematics.objects.RealInterval
Quaternion = mathematics.geometry.d3.transformations.rotations.Quaternion
Length = physics.units.Length
Angle = physics.units.Angle
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Duration = physics.time.Duration
Instant = physics.time.Instant
Transform = physics.coordinate.Transform
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.providers.Dynamic
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth
Trajectory = astrodynamics.Trajectory
Profile = astrodynamics.flight.Profile
State = astrodynamics.flight.profile.State
SatelliteSystem = astrodynamics.flight.system.SatelliteSystem
SatelliteDynamics = astrodynamics.flight.system.dynamics.SatelliteDynamics
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.sgp4
Access = astrodynamics.Access

################################################################################################################################################################

# def test_trajectory_constructors ():

################################################################################################################################################################

# def test_trajectory_comparators ():

################################################################################################################################################################

def test_trajectory_undefined ():

    trajectory: Trajectory = Trajectory.undefined()

    assert trajectory is not None
    assert isinstance(trajectory, Trajectory)
    assert trajectory.is_defined() is False

################################################################################################################################################################

# def test_trajectory_position ():

################################################################################################################################################################

# def test_trajectory_get_state_at ():
# [TBI] add actual unit tests

################################################################################################################################################################

# def test_trajectory_get_states_at ():
# [TBI] add actual unit tests to test sorting and array lengths, can be done with a propagated object

################################################################################################################################################################
