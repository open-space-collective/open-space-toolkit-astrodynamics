# Apache License 2.0

import pytest

from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics import Environment

from ostk.astrodynamics.trajectory.orbit import Pass

earth = Environment.default().get_celestial_object_with_name("Earth")


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
    def test_is_complete(self, pass_: Pass):
        assert pass_.is_complete() is not None

    def test_get_type(self, pass_: Pass):
        assert pass_.get_type() is not None

    def test_get_revolution_number(self, pass_: Pass):
        assert pass_.get_revolution_number() is not None
        assert pass_.get_revolution_number() == 123

    def test_constructor_without_revolution_number(self):
        instants: list[Instant] = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 15, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 30, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 45, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 1, 0, 0), Scale.UTC),
        ]

        pass_without_revolution_number = Pass(None, *instants)

        assert pass_without_revolution_number.get_type() == Pass.Type.Complete
        assert pass_without_revolution_number.get_revolution_number() is None

        partial_pass = Pass(123, *([None] * 5))

        assert partial_pass.get_type() == Pass.Type.Partial
        assert partial_pass.get_revolution_number() == 123

    def test_constructor_partial_pass(self, pass_: Pass):
        partial_pass = Pass(
            123,
            None,
            pass_.get_instant_at_north_point(),
            pass_.get_instant_at_descending_node(),
            None,
            None,
        )

        assert partial_pass.is_complete() is False
        assert partial_pass.get_type() == Pass.Type.Partial
        assert partial_pass.get_instant_at_ascending_node() is None
        assert partial_pass.get_instant_at_south_point() is None
        assert partial_pass.get_instant_at_pass_break() is None
        assert partial_pass.get_start_instant() == pass_.get_instant_at_north_point()
        assert partial_pass.get_end_instant() == pass_.get_instant_at_descending_node()
        assert partial_pass.get_duration() is None
        assert partial_pass.get_interval() is None

    def test_constructor_without_instants(self):
        pass_without_instants = Pass(123, None, None, None, None, None)

        assert pass_without_instants.get_start_instant() is None
        assert pass_without_instants.get_end_instant() is None

    def test_get_duration(self, pass_: Pass):
        assert pass_.get_duration() is not None

    def test_get_start_instant(self, pass_: Pass):
        assert pass_.get_start_instant() is not None

    def test_get_end_instant(self, pass_: Pass):
        assert pass_.get_end_instant() is not None

    def test_get_interval(self, pass_: Pass):
        assert pass_.get_interval() is not None

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

    def test_string_from_type(self):
        assert Pass.string_from_type(Pass.Type.Complete) is not None

    def test_string_from_phase(self):
        assert Pass.string_from_phase(Pass.Phase.Ascending) is not None

    def test_type_has_no_undefined(self):
        assert not hasattr(Pass.Type, "Undefined")

    def test_phase_has_no_undefined(self):
        assert not hasattr(Pass.Phase, "Undefined")
