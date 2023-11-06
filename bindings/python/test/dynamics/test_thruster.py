# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale

from ostk.astrodynamics.trajectory.state import CoordinatesBroker

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.guidance_law import ConstantThrust


@pytest.fixture
def guidance_law() -> ConstantThrust:
    return ConstantThrust.intrack(True)


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
def position_coordinates() -> list[float]:
    return [7000000.0, 0.0, 0.0]


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

    def test_compute_acceleration_success(
        self,
        instant: Instant,
        position_coordinates: list[float],
        velocity_coordinates: list[float],
        thrust_acceleration: float,
    ):
        contribution = guidance_law.compute_acceleration(
            instant,
            position_coordinates,
            velocity_coordinates,
            thrust_acceleration,
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx([0.0, 0.009523809523809525, 0.0], abs=5e-11)
