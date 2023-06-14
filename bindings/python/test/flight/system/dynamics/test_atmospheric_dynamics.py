# Apache License 2.0

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
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system.dynamics import AtmosphericDynamics


@pytest.fixture
def earth() -> Earth:
    earth_WGS84 = Earth.WGS84(20, 0)
    return Earth(
        earth.get_gravitational_parameter(),
        earth.get_equatorial_radius(),
        0.0,
        0.0,
        0.0,
        earth.access_ephemeris(),
        earth.access_gravitational_model().get_type(),
        None,
        EarthAtmosphericModel.EarthAtmosphericType.Exponential,
        Instant.J2000(),
    )


@pytest.fixture
def satellite_system() -> SatelliteSystem:
    mass = Mass(100.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.ndarray(shape=(3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2

    return SatelliteSystem(
        mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient
    )


@pytest.fixture
def dynamics(earth: Earth, satellite_system: SatelliteSystem) -> AtmosphericDynamics:
    return AtmosphericDynamics(earth, satellite_system)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )
    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestAtmosphericDynamics:
    def test_constructors(self, dynamics: AtmosphericDynamics):
        assert dynamics is not None
        assert isinstance(dynamics, AtmosphericDynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: AtmosphericDynamics, earth: Earth):
        assert dynamics.get_celestial() == earth

    def test_update(self, dynamics: AtmosphericDynamics, state: State):
        dxdt: np.ndarray = np.zeros(6)
        dynamics.update(state.get_coordinates(), dxdt, state.get_instant())
        assert True
