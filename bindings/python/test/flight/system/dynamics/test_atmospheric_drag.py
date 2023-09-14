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

from ostk.astrodynamics.trajectory.state import CoordinatesSubset
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state import CoordinatesBroker

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
def dry_mass() -> float:
    return 100.0


@pytest.fixture
def satellite_system(dry_mass: float) -> SatelliteSystem:
    mass = Mass(dry_mass, Mass.Unit.Kilogram)
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.ndarray(shape=(3, 3))
    surface_area = 500.0
    drag_coefficient = 2.1

    return SatelliteSystem(
        mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient
    )


@pytest.fixture
def dynamics(earth: Earth, satellite_system: SatelliteSystem) -> AtmosphericDrag:
    return AtmosphericDrag(earth, satellite_system)


@pytest.fixture
def coordinates_broker() -> CoordinatesBroker:
    return CoordinatesBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinatesSubset.mass(),
        ]
    )


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)


@pytest.fixture
def position_coordinates() -> list:
    return [7000000.0, 0.0, 0.0]


@pytest.fixture
def velocity_coordinates() -> list:
    return [0.0, 7546.05329, 0.0]


@pytest.fixture
def state(
    instant: Instant,
    position_coordinates: list,
    velocity_coordinates: list,
    dry_mass: float,
    coordinates_broker: CoordinatesBroker,
) -> State:
    wet_mass = dry_mass + 10.0
    coordinates = position_coordinates + velocity_coordinates + [wet_mass]
    return State(instant, coordinates, Frame.GCRF(), coordinates_broker)


class TestAtmosphericDrag:
    def test_constructors(self, dynamics: AtmosphericDrag):
        assert dynamics is not None
        assert isinstance(dynamics, AtmosphericDrag)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: AtmosphericDrag, earth: Earth):
        assert dynamics.get_celestial() == earth

    def test_compute_contribution(self, dynamics: AtmosphericDrag, state: State):
        contribution = dynamics.compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx(
            [0.0, -2.53370345e-05, 3.57473816e-12], abs=5e-11
        )
