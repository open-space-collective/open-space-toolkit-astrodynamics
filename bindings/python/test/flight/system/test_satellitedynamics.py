################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/system/test_satellitedynamics.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.physics as physics
import ostk.mathematics as mathematics
import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Cuboid = mathematics.geometry.d3.objects.Cuboid
Point = mathematics.geometry.d3.objects.Point

Length = physics.units.Length
Mass = physics.units.Mass
Derived = physics.units.Derived
Angle = physics.units.Angle
Time = physics.units.Time
Scale = physics.time.Scale
Instant = physics.time.Instant
Interval = physics.time.Interval
DateTime = physics.time.DateTime
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth

State = astrodynamics.trajectory.State
SatelliteSystem = astrodynamics.flight.SatelliteSystem
SatelliteDynamics = astrodynamics.flight.system.SatelliteDynamics

################################################################################################################################################################

def construct_satellitedynamics ():

    satellitesystem = SatelliteSystem.default_yam()

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state:State = State(epoch, position, velocity)

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(Environment.default(), satellitesystem, state, SatelliteDynamics.StateVectorType.Position_velocity)

    assert satellitedynamics is not None
    assert isinstance(satellitedynamics, SatelliteDynamics)
    assert satellitedynamics.is_defined()

    return satellitedynamics

################################################################################################################################################################

def test_flight_system_satellitedynamics_constructors ():

    satellitesystem = SatelliteSystem.default_yam()

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state: State = State(epoch, position, velocity)

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(Environment.default(), satellitesystem, state, SatelliteDynamics.StateVectorType.Position_velocity)

    assert satellitedynamics is not None
    assert isinstance(satellitedynamics, SatelliteDynamics)
    assert satellitedynamics.is_defined()

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(satellitedynamics)

    assert satellitedynamics is not None
    assert isinstance(satellitedynamics, SatelliteDynamics)
    assert satellitedynamics.is_defined()

################################################################################################################################################################

def test_flight_system_satellitedynamics_comparators ():

    satellitedynamics: SatelliteDynamics = construct_satellitedynamics()

    assert satellitedynamics == satellitedynamics
    assert (satellitedynamics != satellitedynamics) is False

################################################################################################################################################################

def test_flight_system_satellitedynamics_getters_setters ():

    satellitedynamics: SatelliteDynamics = construct_satellitedynamics()

    # get_state
    state = State(Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC), Position.meters([6371000.0, 0.0, 0.0], Frame.GCRF()), Velocity.meters_per_second([7600.0, 0.0, 0.0], Frame.GCRF()))

    assert satellitedynamics.get_state() == state

    # set_state

    state_1 = State(Instant.date_time(DateTime(2019, 1, 1, 0, 0, 0), Scale.UTC), Position.meters([6371000.0, 0.0, 0.0], Frame.GCRF()), Velocity.meters_per_second([7600.0, 0.0, 0.0], Frame.GCRF()))

    satellitedynamics.set_state(state_1)

    assert satellitedynamics.is_defined()
    assert satellitedynamics.get_state() == state_1

    # get_integration_stepper_type()

    assert satellitedynamics.get_state_vector_type() == SatelliteDynamics.StateVectorType.Position_velocity

################################################################################################################################################################

def test_flight_system_satellitedynamics_get_string_from_type ():

    assert SatelliteDynamics.string_from_state_vector_type(SatelliteDynamics.StateVectorType.Position_velocity) == 'PositionVelocity'

    assert SatelliteDynamics.string_from_state_vector_type(SatelliteDynamics.StateVectorType.Position_velocity_dragcoeff) == 'PositionVelocitywithDragCoefficient'

################################################################################################################################################################

def test_flight_system_satellitedynamics_calculate_state_at_epoch ():

    satellitedynamics: SatelliteDynamics = construct_satellitedynamics()

    assert satellitedynamics.access_dynamical_equations() is not None #[TBI] add typing to ensure that it return a function pointer?

################################################################################################################################################################
