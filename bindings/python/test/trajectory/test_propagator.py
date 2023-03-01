################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_propagator.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.objects import Cuboid
from ostk.mathematics.geometry.d3.objects import Composite
from ostk.mathematics.geometry.d3.objects import Point

from ostk.physics.units import Mass
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics import Environment
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics import NumericalSolver
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system.dynamics import SatelliteDynamics
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Propagator

################################################################################################################################################################

@pytest.fixture
def propagator_default_inputs ():

    instant_J2000 = Instant.J2000()
    objects = [Earth.WGS84(20, 0),]

    environment = Environment(instant_J2000, objects)

    numericalsolver = NumericalSolver(
        NumericalSolver.LogType.NoLog,
        NumericalSolver.StepperType.RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    )

    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [[1.0, 0.0, 0.0 ], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]], [1.0, 0.0, 0.0]))
    inertia_tensor = np.identity(3)
    surface_area = 0.8
    drag_coefficient = 2.2

    satellitesystem = SatelliteSystem(mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([0.0, 5335.865450622126, 5335.865450622126], frame)

    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state: State = State(instant, position, velocity)

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(environment, satellitesystem)

    return (satellitedynamics, numericalsolver, state)

@pytest.fixture
def propagator (propagator_default_inputs) -> Propagator:

    return Propagator(*propagator_default_inputs[:2])

################################################################################################################################################################

class TestPropagated:

    def test_constructors (self,
                           propagator: Propagator):

        assert propagator is not None
        assert isinstance(propagator, Propagator)
        assert propagator.is_defined()

    def test_calculate_state (self,
                              propagator: Propagator,
                              propagator_default_inputs):

        (_, _, state) = propagator_default_inputs

        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        propagator_state = propagator.calculate_state_at(state, instant)

        propagator_state_position_ref = np.array([6265892.25765909, 3024770.94961259, 3024359.72137468])
        propagator_state_velocity_ref = np.array([-3974.49168221,  4468.16996776,  4466.19232746])

        propagator_state_position = propagator_state.get_position().get_coordinates()
        propagator_state_velocity = propagator_state.get_velocity().get_coordinates()

        assert all([round(propagator_state_position[i], 8) == round(propagator_state_position_ref[i], 8) for i in range(0, len(propagator_state_position_ref))])
        assert all([round(propagator_state_velocity[i], 8) == round(propagator_state_velocity_ref[i], 8) for i in range(0, len(propagator_state_velocity_ref))])
        assert propagator_state.get_instant() == instant

    def test_calculate_states (self,
                               propagator: Propagator,
                               propagator_default_inputs):

        (_, _, state) = propagator_default_inputs

        instant_array = [Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC), Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC)]

        _ = propagator.calculate_states_at(state, instant_array)

        with pytest.raises(RuntimeError):
            instant_array.reverse()
            propagator.calculate_states_at(state, instant_array)

    def test_static_methods (self):

        propagator = Propagator.medium_fidelity()

        assert propagator is not None
        assert isinstance(propagator, Propagator)
        assert propagator.is_defined()

        propagator = Propagator.high_fidelity()

        assert propagator is not None
        assert isinstance(propagator, Propagator)
        assert propagator.is_defined()

################################################################################################################################################################
