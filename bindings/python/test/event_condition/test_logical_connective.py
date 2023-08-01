# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import LogicalConnective


@pytest.fixture
def event_condition() -> EventCondition:
    class MyCondition(EventCondition):
        def evaluate(self, state_vector, time):
            return time

    return MyCondition("My Condition", EventCondition.Criteria.StrictlyNegative)


@pytest.fixture
def event_conditions(event_condition: EventCondition) -> list[EventCondition]:
    return [event_condition]


@pytest.fixture
def logical_connective(event_conditions: list[EventCondition]) -> LogicalConnective:
    return LogicalConnective("Logical Conjunctive", event_conditions)


class TestLogicalConnective:
    def test_constructor(self, event_conditions: list[EventCondition]):
        assert LogicalConnective("Logical Conjunctive", event_conditions) is not None

    def test_get_event_conditions(
        self,
        logical_connective: LogicalConnective,
        event_conditions: list[EventCondition],
    ):
        assert logical_connective.get_event_conditions() == event_conditions
