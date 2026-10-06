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


def plate_vertex_generator(state: State) -> list[Point]:
    # Plate of size 2.0 m x 6.0 m in the X-Y plane
    return [Point(x, y, 0.0) for x in (-1.0, 1.0) for y in (-3.0, 3.0)]


class TestCustom:
    def test_from_inertial_success(self):
        custom = Custom.from_inertial(
            frame=Frame.GCRF(), vertex_generator=cube_vertex_generator
        )

        assert custom is not None
        assert isinstance(custom, Custom)
        assert isinstance(custom, HardBody)

    def test_from_inertial_failure_non_inertial_frame(self):
        with pytest.raises(RuntimeError):
            Custom.from_inertial(
                frame=Frame.ITRF(), vertex_generator=cube_vertex_generator
            )

    def test_from_local_orbital_frame_success(
        self, vnc_factory: LocalOrbitalFrameFactory
    ):
        custom = Custom.from_local_orbital_frame(
            local_orbital_frame_factory=vnc_factory,
            vertex_generator=cube_vertex_generator,
        )

        assert isinstance(custom, Custom)
        assert isinstance(custom, HardBody)

    def test_from_local_orbital_frame_failure_undefined_factory(self):
        with pytest.raises(RuntimeError):
            Custom.from_local_orbital_frame(
                local_orbital_frame_factory=LocalOrbitalFrameFactory.undefined(),
                vertex_generator=cube_vertex_generator,
            )

    def test_get_vertex_generator_success(self, state: State):
        custom = Custom.from_inertial(
            frame=Frame.GCRF(), vertex_generator=cube_vertex_generator
        )

        assert len(custom.get_vertex_generator()(state)) == 8

    def test_calculate_cross_section_at_success(
        self,
        state: State,
        vnc_factory: LocalOrbitalFrameFactory,
    ):
        # Cube: square shadow of side 2.0 m
        cross_section = Custom.from_inertial(
            frame=Frame.GCRF(), vertex_generator=cube_vertex_generator
        ).calculate_cross_section_at(state=state, frame=Frame.GCRF())

        assert isinstance(cross_section, HardBody.CrossSection)
        assert cross_section.get_enclosing_radius() == pytest.approx(math.sqrt(2.0))

        # Plate in the VNC V-N plane. At this state, N is the GCRF Z axis: projected along it, the plate is a
        # segment of length 2.0 m along V
        custom = Custom.from_local_orbital_frame(
            local_orbital_frame_factory=vnc_factory,
            vertex_generator=plate_vertex_generator,
        )

        assert custom.calculate_cross_section_at(
            state=state, frame=Frame.GCRF()
        ).get_enclosing_radius() == pytest.approx(1.0)

        # Projected along the VNC Z axis (C): the whole plate
        assert custom.calculate_cross_section_at(
            state=state, frame=vnc_factory.generate_frame(state)
        ).get_enclosing_radius() == pytest.approx(math.sqrt(10.0))

    def test_calculate_cross_section_at_failure_no_vertex(self, state: State):
        custom = Custom.from_inertial(
            frame=Frame.GCRF(), vertex_generator=lambda state: []
        )

        with pytest.raises(RuntimeError):
            custom.calculate_cross_section_at(state=state, frame=Frame.GCRF())
