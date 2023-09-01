# Apache License 2.0

import pytest

from ostk.physics.time import Instant, DateTime, Scale
from ostk.physics.coordinate import Frame, Position, Velocity

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.event_condition import BooleanCondition


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )

    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


@pytest.fixture
def evaluator():
    return lambda state: state.get_coordinates()[0]


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

    def test_evaluate(self, state: State, event_condition: BooleanCondition):
        assert event_condition.evaluate(state) is not None

    def test_is_satisfied(self, state: State, event_condition: BooleanCondition):
        assert (
            event_condition.is_satisfied(previous_state=state, current_state=state)
            is not None
        )
