# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.time import DateTime, Instant, Scale

from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2023, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def criterion() -> InstantCondition.Criterion:
    return InstantCondition.Criterion.StrictlyPositive


@pytest.fixture
def instant_condition(
    criterion: InstantCondition.Criterion, instant: Instant
) -> InstantCondition:
    return InstantCondition(criterion, instant)


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters(
        [717094.039086306, -6872433.2241124, 46175.9696673281], frame
    )
    velocity: Velocity = Velocity.meters_per_second(
        [-970.650826004612, -45.4598114773158, 7529.82424886455], frame
    )

    instant: Instant = Instant.date_time(DateTime(2023, 1, 1, 0, 1, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestInstantCondition:
    def test_get_instant(self, instant_condition: InstantCondition, instant: Instant):
        assert instant_condition.get_instant() == instant

    def test_evaluate(self, instant_condition: InstantCondition, state: State):
        assert instant_condition.evaluate(state) == 60.0
