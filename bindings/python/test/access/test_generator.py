################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/access/test_generator.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.physics as physics
import ostk.mathematics as mathematics

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
Interval = physics.time.Interval
Transform = physics.coordinate.Transform
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.providers.Dynamic
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth
Trajectory = astrodynamics.Trajectory
Profile = astrodynamics.flight.Profile
State =  astrodynamics.flight.profile.State
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.sgp4
Access = astrodynamics.Access
Generator = astrodynamics.access.Generator

environment: Environment = Environment.default()

################################################################################################################################################################

def test_access_generator_constructors ():

    # Contruction with environment and no filters

    generator: Generator = Generator(environment)

    assert generator is not None
    assert isinstance(generator, Generator)

    # Construction with environment and AER filter
    def aer_filter (aer):

        return True

    generator: Generator = Generator(environment, aer_filter)

    assert generator is not None
    assert isinstance(generator, Generator)

    # Construction iwth environment and both AER and Access Filters
    def access_filter (access):

        return True

    generator: Generator = Generator(environment, aer_filter, access_filter)

    assert generator is not None
    assert isinstance(generator, Generator)

################################################################################################################################################################

def test_access_generator_compute_accesses ():

    # Contruction with environment and no filters

    generator: Generator = Generator(environment)

    start_instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    end_instant = Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC)
    interval = Interval.closed(start_instant, end_instant)

    def generate_first_trajectory ():

        a = Length.kilometers(7000.0)
        e = 0.0
        i = Angle.degrees(45.0)
        raan = Angle.degrees(0.0)
        aop = Angle.degrees(0.0)
        nu = Angle.degrees(0.0)

        coe = COE(a, e, i, raan, aop, nu)

        epoch = start_instant
        earth = Earth.default()

        kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

        return Orbit(kepler, earth)

    def generate_second_trajectory ():

        a = Length.kilometers(7000.0)
        e = 0.0
        i = Angle.degrees(45.0)
        raan = Angle.degrees(180.0)
        aop = Angle.degrees(0.0)
        nu = Angle.degrees(180.0)

        coe = COE(a, e, i, raan, aop, nu)

        epoch = start_instant
        earth = Earth.default()

        kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

        return Orbit(kepler, earth)

    from_trajectory = generate_first_trajectory()
    to_trajectory = generate_second_trajectory()

    accesses = generator.compute_accesses(interval, from_trajectory, to_trajectory)

    assert accesses is not None
    assert isinstance(accesses, list)
    assert accesses[0] is not None
    assert isinstance(accesses[0], Access)

################################################################################################################################################################

def test_access_generator_setters ():

    generator: Generator = Generator(environment)

    # set_step
    step: Duration = Duration(1)

    generator.set_step(step)
    # set_tolerance
    tolerance: Duration = Duration(1)

    generator.set_step(tolerance)

    # set_aer_filter
    def aer_filter (aer):
        return True

    generator.set_aer_filter(aer_filter)

    # set_access_filter
    def access_filter (access):
        return True

    generator.set_access_filter(access_filter)

################################################################################################################################################################

def test_access_generator_undefined ():

    generator: Generator = Generator.undefined()

    assert generator is not None
    assert isinstance(generator, Generator)
    assert generator.is_defined() is False

################################################################################################################################################################

def test_access_generator_aer_ranges ():

    # Construct arbitrary AER ranges
    azimuth_interval = RealInterval.closed(0.0, 360.0)
    elevation_interval = RealInterval.closed(0.0, 90.0)
    range_interval = RealInterval.closed(0.0, 7000e3)

    generator: Generator = Generator.aer_ranges(azimuth_interval, elevation_interval, range_interval, environment)

    assert generator is not None
    assert isinstance(generator, Generator)
    assert generator.is_defined()

################################################################################################################################################################

def test_access_generator_aer_mask ():

    # Construct arbitrary anAzimuthElevationMask using python dict
    an_azimuth_elevation_mask = {0.0: 30.0, 90.0: 60.0, 180.0: 60.0, 270.0: 30.0, 359.0: 30.0}

    # Construct arbitrary aRangerange
    a_range_range = RealInterval(0.0, 10e4, RealInterval.Type.Closed)

    generator: Generator = Generator.aer_mask(an_azimuth_elevation_mask, a_range_range, environment)

    assert generator is not None
    assert isinstance(generator, Generator)
    assert generator.is_defined()

################################################################################################################################################################
