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
from ostk.astrodynamics.trajectory.state import CoordinatesSubset, CoordinatesBroker
from ostk.astrodynamics.trajectory.state.coordinates_subset import (
    CartesianPosition,
    CartesianVelocity,
)
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import AtmosphericDrag


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
def surface_area() -> float:
    return 500.0


@pytest.fixture
def drag_coefficient() -> float:
    return 2.1


@pytest.fixture
def dynamics(earth: Earth) -> AtmosphericDrag:
    return AtmosphericDrag(earth)


@pytest.fixture
def coordinates_broker() -> CoordinatesBroker:
    return CoordinatesBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinatesSubset.mass(),
            CoordinatesSubset.surface_area(),
            CoordinatesSubset.drag_coefficient(),
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
    surface_area: float,
    drag_coefficient: float,
    coordinates_broker: CoordinatesBroker,
) -> State:
    coordinates = (
        position_coordinates
        + velocity_coordinates
        + [dry_mass, surface_area, drag_coefficient]
    )
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
