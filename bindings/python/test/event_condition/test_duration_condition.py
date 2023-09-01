# Apache License 2.0

import pytest

from ostk.physics.time import Duration

from ostk.astrodynamics.event_condition import InstantCondition


@pytest.fixture
def duration() -> Duration:
    return Duration.seconds(5.0)


@pytest.fixture
def criterion() -> InstantCondition.Criterion:
    return InstantCondition.Criterion.StrictlyPositive


@pytest.fixture
def duration_condition(
    criterion: InstantCondition.Criterion, duration: Duration
) -> InstantCondition:
    return InstantCondition(criterion, duration)


class TestInstantCondition:
    def test_get_duration(self, duration_condition: InstantCondition, duration: Duration):
        assert duration_condition.get_duration() == duration

    def test_evaluate(self, duration_condition: InstantCondition, duration: Duration):
        time: float = 3.0
        assert duration_condition.evaluate(state_vector=[], time=time) == (
            time - duration.in_seconds()
        )
