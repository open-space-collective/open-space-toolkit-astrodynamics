# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory


@pytest.fixture
def local_orbital_frame_factory() -> LocalOrbitalFrameFactory:
    return LocalOrbitalFrameFactory.VNC(Frame.GCRF())


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def position_vector() -> list:
    return [7500000.0, 0.0, 0.0]


@pytest.fixture
def velocity_vector() -> list:
    return [0.0, 5335.865450622126, 5335.865450622126]


class TestLocalOrbitalFrameFactory:
    def test_constructors(self):
        assert LocalOrbitalFrameFactory.VNC(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.NED(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.LVLH(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.QSW(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.TNW(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.VVLH(Frame.GCRF()) is not None

    def test_generate_frame(
        self,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
        instant: Instant,
        position_vector: list,
        velocity_vector: list,
    ):
        frame = local_orbital_frame_factory.generate_frame(
            instant,
            position_vector,
            velocity_vector,
        )

        assert frame is not None
        assert frame.is_defined()

    def test_is_defined(
        self,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
    ):
        assert local_orbital_frame_factory.is_defined()
