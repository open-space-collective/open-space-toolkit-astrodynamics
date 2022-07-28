################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/system/dynamics/test_satellite_dynamics.py
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
def satellite_dynamics_default_inputs_fix ():

    environment = Environment.default()

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

    return environment, satellitesystem, state

@pytest.fixture
def satellitedynamics_fix (satellite_dynamics_default_inputs_fix) -> SatelliteDynamics:

    environment, satellitesystem, state = satellite_dynamics_default_inputs_fix

    satellitedynamics: SatelliteDynamics = SatelliteDynamics(environment, satellitesystem, state)

    return satellitedynamics

################################################################################################################################################################

class TestSatelliteDynamics:

    def test_constructors (self, satellitedynamics_fix: SatelliteDynamics):

        assert satellitedynamics_fix is not None
        assert isinstance(satellitedynamics_fix, SatelliteDynamics)
        assert satellitedynamics_fix.is_defined()

        satellitedynamics: SatelliteDynamics = SatelliteDynamics(satellitedynamics_fix)

        assert satellitedynamics is not None
        assert isinstance(satellitedynamics, SatelliteDynamics)
        assert satellitedynamics.is_defined()

    def test_comparators (self, satellitedynamics_fix: SatelliteDynamics):

        assert satellitedynamics_fix == satellitedynamics_fix
        assert (satellitedynamics_fix != satellitedynamics_fix) is False

    def test_setters (self, satellite_dynamics_default_inputs_fix, satellitedynamics_fix: SatelliteDynamics):
        environment, satellitesystem, state = satellite_dynamics_default_inputs_fix

        # get_state
        assert satellitedynamics_fix.get_state() == state

        # set_state
        state_1 = State(Instant.date_time(DateTime(2019, 1, 1, 0, 0, 0), Scale.UTC), Position.meters([6371000.0, 0.0, 0.0], Frame.GCRF()), Velocity.meters_per_second([7600.0, 0.0, 0.0], Frame.GCRF()))

        satellitedynamics_fix.set_state(state_1)

        assert satellitedynamics_fix.is_defined()
        assert satellitedynamics_fix.get_state() == state_1

    def test_calculate_state_at_epoch (self, satellitedynamics_fix: SatelliteDynamics):

        assert satellitedynamics_fix.get_dynamical_equations() is not None #[TBI] add typing to ensure that it return a function pointer?

################################################################################################################################################################
