# Apache License 2.0

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

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
State = astrodynamics.flight.profile.State
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.sgp4
Access = astrodynamics.Access


def test_access_constructors():
    acquisition_of_signal = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    time_at_closest_approach = Instant.date_time(DateTime(2018, 1, 1, 0, 1, 0), Scale.UTC)
    loss_of_signal = Instant.date_time(DateTime(2018, 1, 1, 0, 2, 0), Scale.UTC)
    max_elevation = Angle.degrees(54.3)

    access: Access = Access(
        Access.Type.Complete,
        acquisition_of_signal,
        time_at_closest_approach,
        loss_of_signal,
        max_elevation,
    )

    assert access is not None
    assert isinstance(access, Access)


def test_access_undefined():
    access: Access = Access.undefined()

    assert access is not None
    assert isinstance(access, Access)
    assert access.is_defined() is False


def test_access_getters():
    acquisition_of_signal = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    time_at_closest_approach = Instant.date_time(DateTime(2018, 1, 1, 0, 1, 0), Scale.UTC)
    loss_of_signal = Instant.date_time(DateTime(2018, 1, 1, 0, 2, 0), Scale.UTC)
    max_elevation = Angle.degrees(54.3)

    access: Access = Access(
        Access.Type.Complete,
        acquisition_of_signal,
        time_at_closest_approach,
        loss_of_signal,
        max_elevation,
    )

    # get_type
    access_type: Access.Type = access.get_type()

    assert access_type is not None
    assert isinstance(access_type, Access.Type)
    assert access_type == Access.Type.Complete

    # get_acquisition_of_signal
    acq_signal: Instant = access.get_acquisition_of_signal()

    assert acq_signal is not None
    assert isinstance(acq_signal, Instant)
    assert acq_signal == acquisition_of_signal

    # get_time_of_closest_approach
    closest_approach: Instant = access.get_time_of_closest_approach()

    assert closest_approach is not None
    assert isinstance(closest_approach, Instant)
    assert closest_approach == time_at_closest_approach

    # get_loss_of_signal
    loss_signal: Instant = access.get_loss_of_signal()

    assert loss_signal is not None
    assert isinstance(loss_signal, Instant)
    assert loss_signal == loss_of_signal

    # get_interval
    interval = access.get_interval()
    assert interval is not None
    assert isinstance(interval, Interval)

    # get_duration
    duration: Duration = access.get_duration()

    assert duration is not None
    assert isinstance(duration, Duration)

    # get_max_elevation
    max_el: Angle = access.get_max_elevation()

    assert max_el is not None
    assert isinstance(max_el, Angle)


def test_access_string_from_type():
    assert Access.string_from_type(Access.Type.Undefined) == "Undefined"
    assert Access.string_from_type(Access.Type.Complete) == "Complete"
    assert Access.string_from_type(Access.Type.Partial) == "Partial"
