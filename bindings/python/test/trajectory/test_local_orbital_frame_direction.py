# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection


@pytest.fixture
def local_orbital_frame_factory() -> LocalOrbitalFrameFactory:
    return LocalOrbitalFrameFactory.VNC(Frame.GCRF())


@pytest.fixture
def direction_vector() -> list:
    return [1.0, 0.0, 0.0]


@pytest.fixture
def local_orbital_frame_direction(
    direction_vector: list,
    local_orbital_frame_factory: LocalOrbitalFrameFactory,
) -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(
        vector=direction_vector,
        local_orbital_frame_factory=local_orbital_frame_factory,
    )


class TestLocalOrbitalFrameDirection:
    def test_constructor(
        self,
        direction_vector: list,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
    ):
        local_orbital_frame_direction: LocalOrbitalFrameDirection = (
            LocalOrbitalFrameDirection(
                direction_vector,
                local_orbital_frame_factory,
            )
        )

        assert local_orbital_frame_direction is not None
        assert isinstance(local_orbital_frame_direction, LocalOrbitalFrameDirection)

    def test_equal_to_operator(
        self, local_orbital_frame_direction: LocalOrbitalFrameDirection
    ):
        assert local_orbital_frame_direction == local_orbital_frame_direction
        assert local_orbital_frame_direction != LocalOrbitalFrameDirection(
            [0.0, 1.0, 0.0],
            local_orbital_frame_direction.get_local_orbital_frame_factory(),
        )

    def test_is_defined(
        self,
        local_orbital_frame_direction: LocalOrbitalFrameDirection,
    ):
        assert local_orbital_frame_direction.is_defined()

    def test_getters(
        self,
        local_orbital_frame_direction: LocalOrbitalFrameDirection,
        direction_vector: list,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
    ):
        assert list(local_orbital_frame_direction.get_value()) == direction_vector
        assert (
            local_orbital_frame_direction.get_local_orbital_frame_factory()
            == local_orbital_frame_factory
        )

    def test_undefined(
        self,
    ):
        local_orbital_frame_direction: LocalOrbitalFrameDirection = (
            LocalOrbitalFrameDirection.undefined()
        )
        assert local_orbital_frame_direction.is_defined() is False
