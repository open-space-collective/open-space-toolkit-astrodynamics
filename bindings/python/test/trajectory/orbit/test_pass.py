# Apache License 2.0

import pytest

from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics import Environment

from ostk.astrodynamics.trajectory.orbit import Pass

earth = Environment.default().access_celestial_object_with_name("Earth")


@pytest.fixture
def pass_() -> Pass:
    return Pass(
        123,
        Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
        Instant.date_time(DateTime(2018, 1, 1, 0, 15, 0), Scale.UTC),
        Instant.date_time(DateTime(2018, 1, 1, 0, 30, 0), Scale.UTC),
        Instant.date_time(DateTime(2018, 1, 1, 0, 45, 0), Scale.UTC),
        Instant.date_time(DateTime(2018, 1, 1, 1, 0, 0), Scale.UTC),
    )


class TestPass:
    def test_is_defined(self, pass_: Pass):
        assert pass_.is_defined()

    def test_is_complete(self, pass_: Pass):
        assert pass_.is_complete() is not None

    def test_get_type(self, pass_: Pass):
        assert pass_.get_type() is not None

    def test_get_revolution_number(self, pass_: Pass):
        assert pass_.get_revolution_number() is not None

    def test_get_duration(self, pass_: Pass):
        assert pass_.get_duration() is not None

    def test_get_instant_at_ascending_node(self, pass_: Pass):
        assert pass_.get_instant_at_ascending_node() is not None

    def test_get_instant_at_north_point(self, pass_: Pass):
        assert pass_.get_instant_at_north_point() is not None

    def test_get_instant_at_descending_node(self, pass_: Pass):
        assert pass_.get_instant_at_descending_node() is not None

    def test_get_instant_at_south_point(self, pass_: Pass):
        assert pass_.get_instant_at_south_point() is not None

    def test_get_instant_at_pass_break(self, pass_: Pass):
        assert pass_.get_instant_at_pass_break() is not None

    def test_undefined(self):
        assert Pass.undefined().is_defined() is False

    def test_string_from_type(self):
        assert Pass.string_from_type(Pass.Type.Complete) is not None

    def test_string_from_phase(self):
        assert Pass.string_from_phase(Pass.Phase.Ascending) is not None
