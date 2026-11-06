# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.conjunction import HardBody
from ostk.astrodynamics.conjunction.hard_body import Spherical


@pytest.fixture
def state() -> State:
    return State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )


@pytest.fixture
def spherical() -> Spherical:
    return Spherical(radius=2.0)


class TestSpherical:
    def test_constructor_success(self, spherical: Spherical):
        assert spherical is not None
        assert isinstance(spherical, Spherical)
        assert isinstance(spherical, HardBody)

    def test_constructor_failure_negative_radius(self):
        with pytest.raises(RuntimeError):
            Spherical(radius=-1.0)

    def test_get_radius_success(self, spherical: Spherical):
        assert spherical.get_radius() == 2.0

    def test_calculate_cross_section_at_success(
        self,
        spherical: Spherical,
        state: State,
    ):
        cross_section = spherical.calculate_cross_section_at(
            state=state, frame=Frame.GCRF()
        )

        assert isinstance(cross_section, HardBody.CrossSection)
        assert cross_section.get_enclosing_radius() == pytest.approx(2.0)

        assert spherical.calculate_cross_section_at(
            state=state, frame=Frame.ITRF()
        ).get_enclosing_radius() == pytest.approx(2.0)
