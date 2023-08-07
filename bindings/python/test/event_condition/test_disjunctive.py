# Apache License 2.0

import pytest

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import Disjunctive


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
def disjunction_condition(event_conditions: list[EventCondition]) -> Disjunctive:
    return Disjunctive(event_conditions)


class TestDisjunctiveCondition:
    def test_constructor(self, event_conditions: list[EventCondition]):
        assert Disjunctive(event_conditions) is not None

    def test_is_satisfied(self, disjunction_condition: Disjunctive):
        # x0 crossing root (true), x1 negative (true)
        assert (
            disjunction_condition.is_satisfied([1.0, 0.0], 0.0, [-1.0, 3.0], 0.0) == True
        )

        # x0 crossing root (true), x1 positive (false)
        assert (
            disjunction_condition.is_satisfied([1.0, 1.0], 0.0, [-1.0, 3.0], 0.0) == True
        )

        # x0 not crossing root (false), x1 negative (true)
        assert (
            disjunction_condition.is_satisfied([-0.5, 1.0], 0.0, [-1.0, 3.0], 0.0)
            == False
        )

        # x0 not crossing root (false), x1 positive (false)
        assert (
            disjunction_condition.is_satisfied([-0.5, 0.0], 0.0, [-1.0, 3.0], 0.0) == True
        )
