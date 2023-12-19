# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition


@pytest.fixture
def name() -> str:
    return "MyEvent"


@pytest.fixture
def evaluator() -> callable:
    return lambda state: 0.0


@pytest.fixture
def target_value() -> float:
    return 0.0


@pytest.fixture
def target(target_value: float) -> EventCondition.Target:
    return EventCondition.Target(target_value, EventCondition.Target.Type.Absolute)


@pytest.fixture
def event_condition(
    name: str, evaluator: callable, target: EventCondition.Target
) -> EventCondition:
    class MyEventCondition(EventCondition):
        def is_satisfied(
            self,
            current_state_vector,
            current_time,
            previous_state_vector,
            previous_time,
        ):
            return current_state_vector[0] > 0.0 and previous_state_vector[0] < 0.0

    return MyEventCondition(name, evaluator, target)


class TestEventCondition:
    def test_subclass(self, event_condition: EventCondition):
        assert event_condition is not None

    def test_get_name(self, event_condition: EventCondition, name: str):
        assert event_condition.get_name() == name

    def test_get_evaluator(self, event_condition: EventCondition):
        assert event_condition.get_evaluator() is not None

    def test_get_target(
        self, event_condition: EventCondition, target: EventCondition.Target
    ):
        assert event_condition.get_target() == target
