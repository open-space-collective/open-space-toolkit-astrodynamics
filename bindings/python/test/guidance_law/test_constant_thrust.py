# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection

from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.guidance_law import ConstantThrust


@pytest.fixture
def local_orbital_frame_direction() -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(
        vector=[1.0, 0.0, 0.0],
        local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
    )


@pytest.fixture
def guidance_law(
    local_orbital_frame_direction: LocalOrbitalFrameDirection,
) -> ConstantThrust:
    return ConstantThrust(thrust_direction=local_orbital_frame_direction)


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)


@pytest.fixture
def position_coordinates() -> list[float]:
    return [7000000.0, 0.0, 0.0]


@pytest.fixture
def velocity_coordinates() -> list[float]:
    return [0.0, 7546.05329, 0.0]


@pytest.fixture
def thrust_acceleration() -> float:
    return 1.0 / 105.0


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


class TestConstantThrust:
    def test_constructors(self, guidance_law: ConstantThrust):
        assert guidance_law is not None
        assert isinstance(guidance_law, ConstantThrust)
        assert isinstance(guidance_law, GuidanceLaw)

    def test_getters(self, guidance_law: ConstantThrust):
        assert guidance_law.get_local_thrust_direction() is not None

    def test_static_constructors(self):
        assert ConstantThrust.intrack() is not None

        assert ConstantThrust.intrack(velocity_direction=False) is not None

        assert ConstantThrust.intrack(velocity_direction=True) is not None

    def test_compute_acceleration_success(
        self,
        guidance_law: ConstantThrust,
        instant: Instant,
        position_coordinates: list[float],
        velocity_coordinates: list[float],
        thrust_acceleration: float,
        frame: Frame,
    ):
        contribution = guidance_law.calculate_thrust_acceleration_at(
            instant=instant,
            position_coordinates=position_coordinates,
            velocity_coordinates=velocity_coordinates,
            thrust_acceleration=thrust_acceleration,
            output_frame=frame,
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx([0.0, 0.009523809523809525, 0.0], abs=5e-11)
