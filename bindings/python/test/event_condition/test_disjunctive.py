# Apache License 2.0

import pytest

from ostk.astrodynamics.event_condition import Disjunctive, RealEventCondition


@pytest.fixture
def first_condition() -> RealEventCondition:
    return RealEventCondition(
        "First Condition",
        RealEventCondition.Criteria.PositiveCrossing,
        lambda state_vector, time: state_vector[0],
        0.0,
    )


@pytest.fixture
def second_condition() -> RealEventCondition:
    return RealEventCondition(
        "Second condition",
        RealEventCondition.Criteria.StrictlyNegative,
        lambda state_vector, time: state_vector[1],
        0.1,
    )


@pytest.fixture
def event_conditions(
    first_condition: RealEventCondition, second_condition: RealEventCondition
) -> list[RealEventCondition]:
    return [first_condition, second_condition]


@pytest.fixture
def disjunction_condition(event_conditions: list[RealEventCondition]) -> Disjunctive:
    return Disjunctive(event_conditions)


class TestDisjunctiveCondition:
    def test_constructor(self, event_conditions: list[RealEventCondition]):
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
