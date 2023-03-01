################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/system/dynamics/test_satellite_dynamics.py
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
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system.dynamics import SatelliteDynamics

################################################################################################################################################################

@pytest.fixture
def satellite_dynamics_default_inputs () :

    environment = Environment.default()

    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]], [1.0, 0.0, 0.0]))
    inertia_tensor = np.ndarray(shape = (3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2

    satellite_system = SatelliteSystem(mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    state: State = State(epoch, position, velocity)

    return (environment, satellite_system, state)

@pytest.fixture
def satellite_dynamics (satellite_dynamics_default_inputs) -> SatelliteDynamics:

    return SatelliteDynamics(*satellite_dynamics_default_inputs[:-1])

################################################################################################################################################################

class TestSatelliteDynamics:

    def test_constructor_success (self, satellite_dynamics: SatelliteDynamics):

        assert satellite_dynamics is not None
        assert isinstance(satellite_dynamics, SatelliteDynamics)
        assert satellite_dynamics.is_defined()

    def test_comparators_success (self, satellite_dynamics: SatelliteDynamics):

        assert (satellite_dynamics == satellite_dynamics) is True
        assert (satellite_dynamics != satellite_dynamics) is False

    def test_getters_setters_success (self, satellite_dynamics_default_inputs, satellite_dynamics: SatelliteDynamics):

        (_, _, state) = satellite_dynamics_default_inputs

        satellite_dynamics.set_instant(state.get_instant())

        assert satellite_dynamics.get_instant() == state.get_instant()

        assert satellite_dynamics.is_defined()

    def test_get_dynamical_equations (self, satellite_dynamics: SatelliteDynamics):

        with pytest.raises(RuntimeError):
            satellite_dynamics.get_dynamical_equations()

        satellite_dynamics.set_instant(Instant.J2000())

        assert satellite_dynamics.get_dynamical_equations() is not None  # Returns "<function PyCapsule.>" builtin_function_or_method

################################################################################################################################################################
