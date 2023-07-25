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
        def evaluate(self, state_vector, time):
            return state_vector[0]

    return MyEventCondition(name, criteria)


@pytest.fixture
def event_condition_overloaded() -> EventCondition:
    class OverloadedEventCondition(EventCondition):
        def is_satisfied(self, current_value, previous_value):
            return (current_value > 0.0) and (previous_value < 0.0)

        def evaluate(self, aStateVector, aTime):
            return aStateVector[0]

    return OverloadedEventCondition(
        "OverloadedEventCondition", EventCondition.Criteria.Undefined
    )


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
            event_condition.is_satisfied(current_value=1.0, previous_value=-1.0) == True
        )

    def test_overloaded_is_satisfied(self, event_condition_overloaded: EventCondition):
        assert event_condition_overloaded is not None
        assert (
            event_condition_overloaded.is_satisfied(
                current_value=1.0, previous_value=-1.0
            )
            == True
        )
        assert (
            event_condition_overloaded.is_satisfied(current_value=1.0, previous_value=1.0)
            == False
        )
