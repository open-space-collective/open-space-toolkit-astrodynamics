# Apache License 2.0

import pytest

from ostk.physics.time import Instant, DateTime, Scale
from ostk.physics.coordinate import Frame, Position, Velocity

from ostk.astrodynamics.trajectory import State

from ostk.astrodynamics.event_condition import RealCondition


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
def target() -> float:
    return 10.0


@pytest.fixture
def event_condition(evaluator, target: float) -> RealCondition:
    return RealCondition(
        "My Condition", RealCondition.Criterion.PositiveCrossing, evaluator, target
    )


class TestRealCondition:
    def test_get_target(self, event_condition: RealCondition, target: float):
        assert event_condition.get_target() == target

    def test_evaluate(self, state: State, event_condition: RealCondition):
        assert event_condition.evaluate(state) is not None

    def test_is_satisfied(self, state: State, event_condition: RealCondition):
        assert (
            event_condition.is_satisfied(previous_state=state, current_state=state)
            is not None
        )
