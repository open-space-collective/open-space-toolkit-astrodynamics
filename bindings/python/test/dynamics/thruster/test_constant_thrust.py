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
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection

from ostk.astrodynamics.trajectory.state import CoordinatesSubset
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state import CoordinatesBroker

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics.thruster import ConstantThrust


@pytest.fixture
def local_orbital_frame_direction() -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(
        vector=[1.0, 0.0, 0.0],
        local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
    )


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
    local_orbital_frame_direction: LocalOrbitalFrameDirection,
) -> ConstantThrust:
    return ConstantThrust(
        satellite_system=satellite_system,
        thrust_direction=local_orbital_frame_direction,
        name="Constant Thrust Thruster Dynamics",
    )


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
def state(coordinates_broker: CoordinatesBroker) -> State:
    instant: Instant = Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)
    coordinates: list = [7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 105.0]

    return State(instant, coordinates, Frame.GCRF(), coordinates_broker)


class TestConstantThrust:
    def test_constructors(self, dynamics: ConstantThrust):
        assert dynamics is not None
        assert isinstance(dynamics, ConstantThrust)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: ConstantThrust):
        assert dynamics.get_local_thrust_direction() is not None

    def test_static_constructors(self, satellite_system: SatelliteSystem, frame: Frame):
        assert ConstantThrust.intrack(satellite_system=satellite_system) is not None

        assert (
            ConstantThrust.intrack(
                satellite_system=satellite_system,
                velocity_direction=False,
            )
            is not None
        )

        assert (
            ConstantThrust.intrack(
                satellite_system=satellite_system,
                velocity_direction=True,
                frame=frame,
            )
            is not None
        )

    def test_compute_contribution_success(self, dynamics: ConstantThrust, state: State):
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
        coordinates_broker: CoordinatesBroker,
        dynamics: ConstantThrust,
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
        state = State(instant, coordinates, Frame.GCRF(), coordinates_broker)

        with pytest.raises(RuntimeError):
            contribution = dynamics.compute_contribution(
                state.get_instant(), state.get_coordinates(), state.get_frame()
            )
