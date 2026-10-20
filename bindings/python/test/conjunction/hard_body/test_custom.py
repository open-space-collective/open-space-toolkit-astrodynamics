# Apache License 2.0

import math

import pytest

from ostk.mathematics.geometry.d3.object import Point

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.conjunction import HardBody
from ostk.astrodynamics.conjunction.hard_body import Custom


@pytest.fixture
def state() -> State:
    return State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )


@pytest.fixture
def vnc_factory() -> LocalOrbitalFrameFactory:
    return LocalOrbitalFrameFactory.VNC(Frame.GCRF())


def cube_vertex_generator(state: State) -> list[Point]:
    # Cube of side 2.0 m
    return [Point(x, y, z) for x in (-1.0, 1.0) for y in (-1.0, 1.0) for z in (-1.0, 1.0)]


class TestCustom:
    def test_constructor_success_frame(self):
        custom = Custom(vertex_generator=cube_vertex_generator, frame=Frame.GCRF())

        assert custom is not None
        assert isinstance(custom, Custom)
        assert isinstance(custom, HardBody)

    def test_constructor_success_local_orbital_frame_factory(
        self, vnc_factory: LocalOrbitalFrameFactory
    ):
        custom = Custom(
            vertex_generator=cube_vertex_generator,
            local_orbital_frame_factory=vnc_factory,
        )

        assert isinstance(custom, Custom)
        assert isinstance(custom, HardBody)

    def test_get_vertex_generator_success(self, state: State):
        custom = Custom(vertex_generator=cube_vertex_generator, frame=Frame.GCRF())

        assert len(custom.get_vertex_generator()(state)) == 8

    def test_calculate_cross_section_at_success(self, state: State):
        # Cube: square shadow of side 2.0 m
        cross_section = Custom(
            vertex_generator=cube_vertex_generator, frame=Frame.GCRF()
        ).calculate_cross_section_at(state=state, frame=Frame.GCRF())

        assert isinstance(cross_section, HardBody.CrossSection)
        assert cross_section.get_enclosing_radius() == pytest.approx(math.sqrt(2.0))

    def test_calculate_cross_section_at_failure_no_vertex(self, state: State):
        custom = Custom(vertex_generator=lambda state: [], frame=Frame.GCRF())

        with pytest.raises(RuntimeError):
            custom.calculate_cross_section_at(state=state, frame=Frame.GCRF())
