# Apache License 2.0

import math

import pytest

from ostk.mathematics.geometry.d2.object import Point

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.conjunction import HardBody


@pytest.fixture
def state() -> State:
    return State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )


@pytest.fixture
def circle() -> HardBody.CrossSection:
    return HardBody.CrossSection([Point(0.0, 0.0)], 1.0)


@pytest.fixture
def square() -> HardBody.CrossSection:
    return HardBody.CrossSection(
        [Point(0.5, 0.5), Point(0.5, -0.5), Point(-0.5, -0.5), Point(-0.5, 0.5)], 0.0
    )


@pytest.fixture
def stadium() -> HardBody.CrossSection:
    return HardBody.CrossSection([Point(-1.0, 0.0), Point(1.0, 0.0)], 0.5)


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

        return HardBody.CrossSection(
            [
                Point(half_length, half_width),
                Point(half_length, -half_width),
                Point(-half_length, -half_width),
                Point(-half_length, half_width),
            ],
            0.0,
        )


class TestCrossSection:
    def test_constructor_success(self):
        cross_section = HardBody.CrossSection(convex_hull=[Point(0.0, 0.0)], radius=1.0)

        assert cross_section is not None
        assert isinstance(cross_section, HardBody.CrossSection)

    def test_get_enclosing_radius_success(
        self,
        circle: HardBody.CrossSection,
        square: HardBody.CrossSection,
        stadium: HardBody.CrossSection,
    ):
        assert circle.get_enclosing_radius() == pytest.approx(1.0)
        assert square.get_enclosing_radius() == pytest.approx(math.sqrt(0.5))
        assert stadium.get_enclosing_radius() == pytest.approx(1.5)

    def test_get_enclosing_radius_failure_empty_convex_hull(self):
        with pytest.raises(RuntimeError):
            HardBody.CrossSection([], 1.0).get_enclosing_radius()

    def test_combine_success(
        self,
        circle: HardBody.CrossSection,
        square: HardBody.CrossSection,
    ):
        combined_cross_section = HardBody.CrossSection.combine(
            cross_section=circle,
            another_cross_section=square,
        )

        assert isinstance(combined_cross_section, HardBody.CrossSection)
        assert combined_cross_section.get_enclosing_radius() == pytest.approx(
            math.sqrt(0.5) + 1.0
        )

    def test_combine_failure_empty_convex_hull(
        self,
        circle: HardBody.CrossSection,
    ):
        with pytest.raises(RuntimeError):
            HardBody.CrossSection.combine(HardBody.CrossSection([], 1.0), circle)


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
