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
Composite = mathematics.geometry.d3.objects.Composite
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
SatelliteSystem = astrodynamics.flight.system.SatelliteSystem
SatelliteDynamics = astrodynamics.flight.system.dynamics.SatelliteDynamics
State = astrodynamics.trajectory.State
Orbit = astrodynamics.trajectory.Orbit
Propagated = astrodynamics.trajectory.orbit.models.Propagated

################################################################################################################################################################

@pytest.fixture
def propagated_default_inputs ():

    instant_J2000 = Instant.J2000()
    objects = [Earth.WGS84()]

    environment = Environment(instant_J2000, objects)

    numericalsolver = NumericalSolver(NumericalSolver.LogType.NoLog, NumericalSolver.StepperType.RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15)

    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] ))
    inertia_tensor = np.ndarray(shape=(3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2

    satellitesystem = SatelliteSystem(mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state: State = State(epoch, position, velocity)

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(environment, satellitesystem, state)

    return (satellitedynamics, numericalsolver, state, environment)

@pytest.fixture
def propagated (propagated_default_inputs) -> Propagated:

    return Propagated(*propagated_default_inputs[0:-2])

################################################################################################################################################################

class TestPropagated:
    def test_constructors (self, propagated: Propagated, propagated_default_inputs):

        assert propagated is not None
        assert isinstance(propagated, Propagated)
        assert propagated.is_defined()

        env = Environment.default()
        orbit = Orbit(propagated, env.access_celestial_object_with_name('Earth'))

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

        (satellite_dynamics, numerical_solver, state, environment) = propagated_default_inputs
        state_array = [state, state]
        propagated_with_state_array = Propagated(satellite_dynamics, numerical_solver, state_array)

        assert propagated_with_state_array is not None
        assert isinstance(propagated_with_state_array, Propagated)
        assert propagated_with_state_array.is_defined()

    def test_comparators (self, propagated: Propagated):

        assert (propagated == propagated) is True
        assert (propagated != propagated) is False

    def test_getters (self, propagated: Propagated, propagated_default_inputs):

        # get_epoch
        (satellite_dynamics, numerical_solver, state, environment)  = propagated_default_inputs

        assert propagated.get_epoch() == state.get_instant()

        # get_revolution_number_at_epoch()

        assert propagated.get_revolution_number_at_epoch() == 1

    def test_calculate_state_at_epoch (self, propagated: Propagated, propagated_default_inputs):

        (satellite_dynamics, numerical_solver, state, environment)  = propagated_default_inputs

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

    def test_calculate_rev_number_at_epoch (self, propagated: Propagated, propagated_default_inputs):

        (satellite_dynamics, numerical_solver, state, environment)  = propagated_default_inputs

        orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

        epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 1, 40, 0), Scale.UTC)

        assert propagated.calculate_revolution_number_at(epoch) == 2
        assert orbit.get_revolution_number_at(epoch) == 2

    def test_access_cached_state_array (self, propagated: Propagated, propagated_default_inputs):

        (satellite_dynamics, numerical_solver, state, environment)  = propagated_default_inputs

        assert len(propagated.access_cached_state_array()) == 1
        assert propagated.access_cached_state_array()[0] == state
