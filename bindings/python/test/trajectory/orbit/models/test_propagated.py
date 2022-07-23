################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_propagated.py
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

Mass = physics.units.Mass
Instant = physics.time.Instant
Interval = physics.time.Interval
DateTime = physics.time.DateTime
Time = physics.units.Time
Scale = physics.time.Scale
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth

NumericalSolver = astrodynamics.NumericalSolver
SatelliteSystem = astrodynamics.flight.SatelliteSystem
SatelliteDynamics = astrodynamics.flight.system.SatelliteDynamics
State = astrodynamics.trajectory.State
Orbit = astrodynamics.trajectory.Orbit
Propagated = astrodynamics.trajectory.orbit.models.Propagated

################################################################################################################################################################

def construct_propagated ():

    instant_J2000 = Instant.J2000()
    objects = [Earth.WGS84()]

    environment = Environment(instant_J2000, objects)

    numericalsolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    satellitesystem = SatelliteSystem.default_yam()

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    state:State = State(epoch, position, velocity)

    satellitedynamics = SatelliteDynamics(environment, satellitesystem, state, SatelliteDynamics.StateVectorType.Position_velocity)

    propagated = Propagated(satellitedynamics, numericalsolver)

    assert propagated is not None
    assert isinstance(propagated, Propagated)
    assert propagated.is_defined()

    return propagated, environment

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_constructors ():

    environment = Environment.default()

    numericalsolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    satellitesystem = SatelliteSystem.default_yam()

    epoch_0 = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    epoch_1 = Instant.date_time(DateTime(2019, 1, 1, 0, 0, 0), Scale.UTC)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    state_array = [State(epoch_0, position, velocity), State(epoch_1, position, velocity)]


    satellitedynamics = SatelliteDynamics(environment, satellitesystem, State(epoch_0, position, velocity), SatelliteDynamics.StateVectorType.Position_velocity)


    propagated = Propagated(satellitedynamics, numericalsolver)

    assert propagated is not None
    assert isinstance(propagated, Propagated)
    assert propagated.is_defined()

    orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

    assert orbit is not None
    assert isinstance(orbit, Orbit)
    assert orbit.is_defined()

    propagated_with_state_array = Propagated(satellitedynamics, numericalsolver, state_array)

    assert propagated_with_state_array is not None
    assert isinstance(propagated_with_state_array, Propagated)
    assert propagated_with_state_array.is_defined()

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_comparators ():

    propagated, environment = construct_propagated()

    assert propagated == propagated
    assert (propagated != propagated) is False

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_getters ():

    propagated, environment = construct_propagated()

    # get_epoch

    assert propagated.get_epoch() == Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    # get_revolution_number_at_epoch()

    assert propagated.get_revolution_number_at_epoch() == 1

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_calculate_state_at_epoch ():

    propagated, environment = construct_propagated()

    orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

    propagated_state = propagated.calculate_state_at(epoch)
    propagated_state_orbit = orbit.get_state_at(epoch)

    assert propagated_state == propagated_state_orbit

    propagated_state_position_ref = np.array([ 9.66277505e+06,  2.06755709e-04, -5.20957785e+00])
    propagated_state_velocity_ref = np.array([ 3.88382712e+03,  5.27400057e-07, -1.32866852e-02])

    propagated_state_position = propagated_state.get_position().get_coordinates()
    propagated_state_velocity = propagated_state.get_velocity().get_coordinates()

    assert all([round(propagated_state_position[i], -9) == round(propagated_state_position_ref[i], -9) for i in range(0, len(propagated_state_position_ref))])
    assert all([round(propagated_state_velocity[i], -9) == round(propagated_state_velocity_ref[i], -9) for i in range(0, len(propagated_state_velocity_ref))])
    assert propagated_state.get_instant() == epoch

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_calculate_rev_number_at_epoch ():

    propagated, environment = construct_propagated()

    orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 1, 40, 0), Scale.UTC)

    assert propagated.calculate_revolution_number_at(epoch) == 2
    assert orbit.get_revolution_number_at(epoch) == 2

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_access_cached_state_array ():

    propagated, environment = construct_propagated()

    assert len(propagated.access_cached_state_array()) == 1
    assert propagated.access_cached_state_array()[0] == State(Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC), Position.meters([6371000.0, 0.0, 0.0], Frame.GCRF()), Velocity.meters_per_second([7600.0, 0.0, 0.0], Frame.GCRF()))

################################################################################################################################################################
