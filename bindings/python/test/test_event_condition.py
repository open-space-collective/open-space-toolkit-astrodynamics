# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition


@pytest.fixture
def name() -> str:
    return "MyEvent"


@pytest.fixture
def event_condition(name: str) -> EventCondition:
    class MyEventCondition(EventCondition):
        def is_satisfied(
            self, current_state_vector, current_time, previous_state_vector, previous_time
        ):
            return current_state_vector[0] > 0.0 and previous_state_vector[0] < 0.0

    return MyEventCondition(name)


class TestEventCondition:
    def test_subclass(self, event_condition: EventCondition):
        assert event_condition is not None

    def test_get_name(self, event_condition: EventCondition, name: str):
        assert event_condition.get_name() == name
