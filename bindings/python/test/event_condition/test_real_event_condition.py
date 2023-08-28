# Apache License 2.0

import pytest

from ostk.astrodynamics.event_condition import RealEventCondition


@pytest.fixture
def evaluator():
    return lambda state_vector, time: time


@pytest.fixture
def target() -> float:
    return 10.0


@pytest.fixture
def event_condition(evaluator, target: float) -> RealEventCondition:
    return RealEventCondition(
        "My Condition", RealEventCondition.Criteria.PositiveCrossing, evaluator, target
    )


class TestRealEventCondition:
    def test_get_target(self, event_condition: RealEventCondition, target: float):
        assert event_condition.get_target() == target

    def test_evaluate(self, event_condition: RealEventCondition):
        assert event_condition.evaluate(state_vector=[0.0], time=10.0) is not None

    def test_is_satisfied(self, event_condition: RealEventCondition):
        assert (
            event_condition.is_satisfied(
                previous_state_vector=[-1.0],
                previous_time=5.0,
                current_state_vector=[1.0],
                current_time=15.0,
            )
            == True
        )
