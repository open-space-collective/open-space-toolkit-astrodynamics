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
from ostk.astrodynamics.conjunction import HardBody
from ostk.astrodynamics.conjunction.hard_body import Custom


@pytest.fixture
def state() -> State:
    return State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )


class Rectangle(HardBody):
    def __init__(self, length: float, width: float):
        super().__init__()
        self._length = length
        self._width = width

    def calculate_cross_section_at(
        self, state: State, frame: Frame
    ) -> HardBody.CrossSection:
        half_length = self._length / 2.0
        half_width = self._width / 2.0

        # Cross sections cannot be constructed directly: delegate to a hard body
        return Custom(
            vertex_generator=lambda state: [
                Point(x, y, 0.0)
                for x in (-half_length, half_length)
                for y in (-half_width, half_width)
            ],
            frame=frame,
        ).calculate_cross_section_at(state=state, frame=frame)


class TestHardBody:
    def test_subclass_success(self, state: State):
        rectangle = Rectangle(length=4.0, width=2.0)

        assert isinstance(rectangle, HardBody)

        cross_section = rectangle.calculate_cross_section_at(
            state=state, frame=Frame.GCRF()
        )

        assert isinstance(cross_section, HardBody.CrossSection)
        assert cross_section.get_enclosing_radius() == pytest.approx(math.sqrt(5.0))

    def test_calculate_cross_section_at_failure_not_implemented(self, state: State):
        with pytest.raises(RuntimeError):
            HardBody().calculate_cross_section_at(state=state, frame=Frame.GCRF())
