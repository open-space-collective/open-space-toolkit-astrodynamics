# Apache License 2.0

from typing import Callable

import pytest

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Transform
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameTransformProvider


@pytest.fixture
def parent_frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def local_orbital_transform_provider_type() -> LocalOrbitalFrameTransformProvider.Type:
    return LocalOrbitalFrameTransformProvider.Type.VNC


@pytest.fixture
def transform_generator() -> Callable:
    return lambda state: Transform.identity(Transform.Type.passive)


@pytest.fixture
def local_orbital_frame_factory(parent_frame: Frame) -> LocalOrbitalFrameFactory:
    return LocalOrbitalFrameFactory.VNC(parent_frame)


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def position() -> Position:
    return Position.meters([7500000.0, 0.0, 0.0], Frame.GCRF())


@pytest.fixture
def velocity() -> Velocity:
    return Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], Frame.GCRF()
    )


@pytest.fixture
def state(
    instant: Instant,
    position: Position,
    velocity: Velocity,
) -> State:
    return State(instant, position, velocity)


class TestLocalOrbitalFrameFactory:
    def test_constructors(self):
        assert LocalOrbitalFrameFactory.VNC(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.NED(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.LVLH(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.QSW(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.TNW(Frame.GCRF()) is not None
        assert LocalOrbitalFrameFactory.VVLH(Frame.GCRF()) is not None

    def test_accessors(
        self,
        parent_frame: Frame,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
    ):
        assert parent_frame == local_orbital_frame_factory.access_parent_frame()
        assert local_orbital_frame_factory.get_provider_type() is not None

    def test_generate_frame(
        self,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
        state: State,
    ):
        frame = local_orbital_frame_factory.generate_frame(state=state)

        assert frame is not None
        assert frame.is_defined()

    def test_is_defined(
        self,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
    ):
        assert local_orbital_frame_factory.is_defined()

    def test_constructor(
        self,
        local_orbital_transform_provider_type: LocalOrbitalFrameTransformProvider.Type,
        parent_frame: Frame,
    ):
        assert (
            LocalOrbitalFrameFactory.construct(
                local_orbital_transform_provider_type, parent_frame
            )
            is not None
        )

    def test_custom_constructor(
        self,
        transform_generator: Callable,
        parent_frame: Frame,
    ):
        assert (
            LocalOrbitalFrameFactory.construct(transform_generator, parent_frame)
            is not None
        )
