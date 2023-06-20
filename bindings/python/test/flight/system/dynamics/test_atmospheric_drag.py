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
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system import Dynamics
from ostk.astrodynamics.flight.system.dynamics import AtmosphericDrag


@pytest.fixture
def earth() -> Earth:
    return Earth.from_models(
        EarthGravitationalModel(EarthGravitationalModel.Type.Undefined),
        EarthMagneticModel(EarthMagneticModel.Type.Undefined),
        EarthAtmosphericModel(EarthAtmosphericModel.Type.Exponential),
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
    surface_area = 1.0
    drag_coefficient = 2.1

    return SatelliteSystem(
        mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient
    )


@pytest.fixture
def dynamics(earth: Earth, satellite_system: SatelliteSystem) -> AtmosphericDrag:
    return AtmosphericDrag(earth, satellite_system)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6900000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )
    instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestAtmosphericDrag:
    def test_constructors(self, dynamics: AtmosphericDrag):
        assert dynamics is not None
        assert isinstance(dynamics, AtmosphericDrag)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: AtmosphericDrag, earth: Earth):
        assert dynamics.get_celestial() == earth

    def test_apply_contribution(self, dynamics: AtmosphericDrag, state: State):
        dxdt: np.ndarray = np.zeros(6)
        dynamics.apply_contribution(state.get_coordinates(), dxdt, state.get_instant())
        assert True
