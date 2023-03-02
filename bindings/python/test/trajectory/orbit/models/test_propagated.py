################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_propagated.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.objects import Cuboid
from ostk.mathematics.geometry.d3.objects import Composite
from ostk.mathematics.geometry.d3.objects import Point

from ostk.physics import Environment
from ostk.physics.units import Mass
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics import NumericalSolver
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system.dynamics import SatelliteDynamics
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.orbit.models import Propagated

################################################################################################################################################################

@pytest.fixture
def propagated_default_inputs ():

    instant_J2000 = Instant.J2000()
    objects = [Earth.WGS84(20, 0)]

    environment = Environment(instant_J2000, objects)

    numericalsolver = NumericalSolver(NumericalSolver.LogType.NoLog, NumericalSolver.StepperType.RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15)

    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] ))
    inertia_tensor = np.ndarray(shape = (3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2

    satellitesystem = SatelliteSystem(mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([0.0, 5335.865450622126, 5335.865450622126], frame)

    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state: State = State(instant, position, velocity)

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(environment, satellitesystem)

    return (satellitedynamics, numericalsolver, state, environment)

@pytest.fixture
def propagated (propagated_default_inputs) -> Propagated:

    return Propagated(*propagated_default_inputs[0:-1])

################################################################################################################################################################

class TestPropagated:

    def test_constructors (self,
                           propagated: Propagated,
                           propagated_default_inputs):

        assert propagated is not None
        assert isinstance(propagated, Propagated)
        assert propagated.is_defined()

        env = Environment.default()
        orbit = Orbit(propagated, env.access_celestial_object_with_name('Earth'))

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

        (satellite_dynamics, numerical_solver, state, _) = propagated_default_inputs
        state_array = [state, state]
        propagated_with_state_array = Propagated(satellite_dynamics, numerical_solver, state_array)

        assert propagated_with_state_array is not None
        assert isinstance(propagated_with_state_array, Propagated)
        assert propagated_with_state_array.is_defined()

    def test_comparators (self,
                          propagated: Propagated):

        assert (propagated == propagated) is True
        assert (propagated != propagated) is False

    def test_getters (self,
                      propagated: Propagated,
                      propagated_default_inputs):

        (_, _, state, _) = propagated_default_inputs

        assert propagated.get_epoch() == state.get_instant()

        assert propagated.get_revolution_number_at_epoch() == 1

    def test_calculate_state_at (self,
                                 propagated: Propagated,
                                 propagated_default_inputs):

        (_, _, _, environment) = propagated_default_inputs

        orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        propagated_state = propagated.calculate_state_at(instant)
        propagated_state_orbit = orbit.get_state_at(instant)

        assert propagated_state == propagated_state_orbit


        propagated_state_position_ref = np.array([6265892.25765909, 3024770.94961259, 3024359.72137468])
        propagated_state_velocity_ref = np.array([-3974.49168221,  4468.16996776,  4466.19232746])

        propagated_state_position = propagated_state.get_position().get_coordinates()
        propagated_state_velocity = propagated_state.get_velocity().get_coordinates()

        assert all([round(propagated_state_position[i], 8) == round(propagated_state_position_ref[i], 8) for i in range(0, len(propagated_state_position_ref))])
        assert all([round(propagated_state_velocity[i], 8) == round(propagated_state_velocity_ref[i], 8) for i in range(0, len(propagated_state_velocity_ref))])
        assert propagated_state.get_instant() == instant

    def test_calculate_states_at (self,
                                  propagated: Propagated,
                                  propagated_default_inputs):

        (_, _, _, environment) = propagated_default_inputs

        orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

        instant_array = [Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC), Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC)]

        propagated_state_array = propagated.calculate_states_at(instant_array)
        propagated_state_array_orbit = orbit.get_states_at(instant_array)

        assert propagated_state_array_orbit == propagated_state_array

        assert propagated_state_array_orbit[0].get_instant() == instant_array[0]
        assert propagated_state_array_orbit[1].get_instant() == instant_array[1]

    def test_calculate_rev_number_at (self,
                                      propagated: Propagated,
                                      propagated_default_inputs):

        (_, _, _, environment) = propagated_default_inputs

        orbit = Orbit(propagated, environment.access_celestial_object_with_name('Earth'))

        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 40, 0), Scale.UTC)

        assert propagated.calculate_revolution_number_at(instant) == 1
        assert orbit.get_revolution_number_at(instant) == 1

    def test_access_cached_state_array (self,
                                        propagated: Propagated,
                                        propagated_default_inputs):

        (_, _, state, _) = propagated_default_inputs

        assert len(propagated.access_cached_state_array()) == 1
        assert propagated.access_cached_state_array()[0] == state

    def test_access_propagator (self,
                                propagated: Propagated):

        assert propagated.access_propagator() is not None
        assert isinstance(propagated.access_propagator(), Propagator)

    def test_set_cached_state_array (self,
                                     propagated: Propagated,
                                     propagated_default_inputs):

        (_, _, state, _) = propagated_default_inputs

        assert len(propagated.access_cached_state_array()) == 1

        propagated.set_cached_state_array([state, state, state])

        assert len(propagated.access_cached_state_array()) == 1
        assert propagated.access_cached_state_array()[0] == state

        with pytest.raises(Exception) as e:
            propagated.set_cached_state_array([])

################################################################################################################################################################
