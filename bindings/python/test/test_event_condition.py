# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition


@pytest.fixture
def name() -> str:
    return "MyEvent"


@pytest.fixture
def criteria() -> EventCondition.Criteria:
    return EventCondition.Criteria.AnyCrossing


@pytest.fixture
def event_condition(name: str, criteria: EventCondition.Criteria) -> EventCondition:
    class MyEventCondition(EventCondition):
        def is_satisfied(
            self, current_state_vector, current_time, previous_state_vector, previous_time
        ):
            return current_state_vector[0] > 0.0 and previous_state_vector[0] < 0.0

    return MyEventCondition(name, criteria)


class TestEventCondition:
    def test_subclass(self, event_condition: EventCondition):
        assert event_condition is not None

    def test_get_name(self, event_condition: EventCondition, name: str):
        assert event_condition.get_name() == name

    def test_get_criteria(
        self, event_condition: EventCondition, criteria: EventCondition.Criteria
    ):
        assert event_condition.get_criteria() == criteria

    def test_is_satisfied(self, event_condition: EventCondition):
        assert (
            event_condition.is_satisfied(
                previous_state_vector=[-1.0],
                previous_time=0.0,
                current_state_vector=[1.0],
                current_time=1.0,
            )
            == True
        )
