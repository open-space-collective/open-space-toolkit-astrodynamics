# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics.unit import Mass
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state import CoordinateBroker

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.guidance_law import ConstantThrust


@pytest.fixture
def guidance_law() -> ConstantThrust:
    return ConstantThrust.intrack(True)


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        150.0,
    )


@pytest.fixture
def satellite_system(propulsion_system: PropulsionSystem) -> SatelliteSystem:
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
    drag_coefficient = 2.1

    return SatelliteSystem(
        mass,
        satellite_geometry,
        inertia_tensor,
        surface_area,
        drag_coefficient,
        propulsion_system,
    )


@pytest.fixture
def dynamics(
    satellite_system: SatelliteSystem,
    guidance_law: ConstantThrust,
) -> Thruster:
    return Thruster(
        satellite_system=satellite_system,
        guidance_law=guidance_law,
        name="Constant Thrust Thruster Dynamics",
    )


@pytest.fixture
def coordinate_broker() -> CoordinateBroker:
    return CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
        ]
    )


@pytest.fixture
def state(coordinate_broker: CoordinateBroker) -> State:
    instant: Instant = Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)
    coordinates: list = [7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 105.0]

    return State(instant, coordinates, Frame.GCRF(), coordinate_broker)


class TestThruster:
    def test_constructors(self, dynamics: Thruster):
        assert dynamics is not None
        assert isinstance(dynamics, Thruster)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: Thruster):
        assert dynamics.get_satellite_system() is not None
        assert dynamics.get_guidance_law() is not None
        assert dynamics.get_read_coordinate_subsets() is not None
        assert dynamics.get_write_coordinate_subsets() is not None

    def test_compute_contribution_success(self, dynamics: Thruster, state: State):
        contribution = dynamics.compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert len(contribution) == 4
        assert contribution == pytest.approx(
            [0.0, 0.009523809523809525, 0.0, -0.0006798108086519521], abs=5e-11
        )

    def test_compute_contribution_failure_out_of_fuel(
        self,
        satellite_system: SatelliteSystem,
        coordinate_broker: CoordinateBroker,
        dynamics: Thruster,
    ):
        instant: Instant = Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)
        coordinates: list = [
            7000000.0,
            0.0,
            0.0,
            0.0,
            7546.05329,
            0.0,
            satellite_system.get_mass().in_kilograms(),
        ]
        state = State(instant, coordinates, Frame.GCRF(), coordinate_broker)

        with pytest.raises(RuntimeError):
            contribution = dynamics.compute_contribution(
                state.get_instant(), state.get_coordinates(), state.get_frame()
            )
