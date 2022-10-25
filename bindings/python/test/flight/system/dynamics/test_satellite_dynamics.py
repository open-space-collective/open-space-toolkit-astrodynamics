################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/system/dynamics/test_satellite_dynamics.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Cuboid = mathematics.geometry.d3.objects.Cuboid
Composite = mathematics.geometry.d3.objects.Composite
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
SatelliteSystem = astrodynamics.flight.system.SatelliteSystem
SatelliteDynamics = astrodynamics.flight.system.dynamics.SatelliteDynamics

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

    return SatelliteDynamics(*satellite_dynamics_default_inputs)

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

        (environment, satellitesystem, state) = satellite_dynamics_default_inputs

        # get_state
        assert satellite_dynamics.get_state() == state

        # set_state
        state_1 = State(Instant.date_time(DateTime(2019, 1, 1, 0, 0, 0), Scale.UTC), Position.meters([6371000.0, 0.0, 0.0], Frame.GCRF()), Velocity.meters_per_second([7600.0, 0.0, 0.0], Frame.GCRF()))

        satellite_dynamics.set_state(state_1)

        assert satellite_dynamics.is_defined()
        assert satellite_dynamics.get_state() == state_1

    def test_calculate_state_at_epoch_success (self, satellite_dynamics: SatelliteDynamics):

        assert satellite_dynamics.get_dynamical_equations() is not None  # [TBI] add typing to ensure that it return a function pointer?

################################################################################################################################################################
