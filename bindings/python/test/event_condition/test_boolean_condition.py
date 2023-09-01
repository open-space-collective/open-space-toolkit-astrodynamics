# Apache License 2.0

import pytest

from ostk.astrodynamics.event_condition import BooleanCondition


@pytest.fixture
def evaluator():
    return lambda state_vector, time: time > 0.0


@pytest.fixture
def is_inversed() -> bool:
    return False


@pytest.fixture
def event_condition(evaluator, is_inversed: bool) -> BooleanCondition:
    return BooleanCondition(
        "My Condition",
        BooleanCondition.Criterion.StrictlyPositive,
        evaluator,
        is_inversed,
    )


class TestBooleanCondition:
    def test_is_inversed(self, event_condition: BooleanCondition, is_inversed: bool):
        assert event_condition.is_inversed() == is_inversed

    def test_evaluate(self, event_condition: BooleanCondition):
        assert event_condition.evaluate(state_vector=[0.0], time=10.0) is not None

    def test_is_satisfied(self, event_condition: BooleanCondition):
        assert (
            event_condition.is_satisfied(
                previous_state_vector=[-1.0],
                previous_time=-1.0,
                current_state_vector=[1.0],
                current_time=10.0,
            )
            == True
        )
