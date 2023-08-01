# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import Conjunctive


@pytest.fixture
def first_condition() -> EventCondition:
    class FirstCondition(EventCondition):
        def evaluate(self, state_vector, time):
            return state_vector[0]

    return FirstCondition("First Condition", EventCondition.Criteria.PositiveCrossing)


@pytest.fixture
def second_condition() -> EventCondition:
    class SecondCondition(EventCondition):
        def evaluate(self, state_vector, time):
            return state_vector[1] - 0.1

    return SecondCondition("Second condition", EventCondition.Criteria.StrictlyNegative)


@pytest.fixture
def event_conditions(
    first_condition: EventCondition, second_condition: EventCondition
) -> list[EventCondition]:
    return [first_condition, second_condition]


@pytest.fixture
def conjunction_condition(event_conditions: list[EventCondition]) -> Conjunctive:
    return Conjunctive(event_conditions)


class TestConjunctiveCondition:
    def test_constructor(self, event_conditions: list[EventCondition]):
        assert Conjunctive(event_conditions) is not None

    def test_is_satisfied(self, conjunction_condition: Conjunctive):
        # x0 crossing root (true), x1 negative (true)
        assert (
            conjunction_condition.is_satisfied([1.0, 0.0], 0.0, [-1.0, 3.0], 0.0) == True
        )

        # x0 crossing root (true), x1 positive (false)
        assert (
            conjunction_condition.is_satisfied([1.0, 1.0], 0.0, [-1.0, 3.0], 0.0) == False
        )

        # x0 not crossing root (false), x1 negative (true)
        assert (
            conjunction_condition.is_satisfied([-0.5, 1.0], 0.0, [-1.0, 3.0], 0.0)
            == False
        )

        # x0 not crossing root (false), x1 positive (false)
        assert (
            conjunction_condition.is_satisfied([-0.5, 0.0], 0.0, [-1.0, 3.0], 0.0)
            == False
        )
