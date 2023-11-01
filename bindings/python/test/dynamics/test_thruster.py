# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinatesSubset
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state import CoordinatesBroker

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
def guidance_law(
    local_orbital_frame_direction: LocalOrbitalFrameDirection,
) -> ConstantThrust:
    return ConstantThrust(thrust_direction=local_orbital_frame_direction)


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)


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
def velocity_coordinates() -> list[float]:
    return [0.0, 7546.05329, 0.0]


@pytest.fixture
def thrust_acceleration() -> float:
    return 1.0


class TestThruster:
    def test_constructors(self, dynamics: Thruster):
        assert dynamics is not None
        assert isinstance(dynamics, Thruster)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_getters(self, dynamics: Thruster):
        assert dynamics.get_satellite_system() is not None
        assert dynamics.get_guidance_law() is not None
        assert dynamics.get_read_coordinates_subsets() is not None
        assert dynamics.get_write_coordinates_subsets() is not None

    def test_compute_contribution_success(self, dynamics: Thruster, state: State):
        contribution = dynamics.compute_contribution(
            state.get_instant(), state.get_coordinates(), state.get_frame()
        )

        assert ConstantThrust.intrack(velocity_direction=False) is not None

        assert ConstantThrust.intrack(velocity_direction=True) is not None

    def test_compute_acceleration_success(
        self,
        satellite_system: SatelliteSystem,
        coordinates_broker: CoordinatesBroker,
        dynamics: Thruster,
    ):
        contribution = guidance_law.compute_acceleration(
            instant,
            position_coordinates,
            velocity_coordinates,
            thrust_acceleration,
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx([0.0, 0.009523809523809525, 0.0], abs=5e-11)
