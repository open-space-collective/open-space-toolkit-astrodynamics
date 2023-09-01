# Apache License 2.0

import pytest

from ostk.physics.time import Duration

from ostk.astrodynamics.event_condition import DurationCondition


@pytest.fixture
def duration() -> Duration:
    return Duration.seconds(5.0)


@pytest.fixture
def criterion() -> DurationCondition.Criterion:
    return DurationCondition.Criterion.StrictlyPositive


@pytest.fixture
def duration_condition(
    criterion: DurationCondition.Criterion, duration: Duration
) -> DurationCondition:
    return DurationCondition(criterion, duration)


class TestDurationCondition:
    def test_get_duration(
        self, duration_condition: DurationCondition, duration: Duration
    ):
        assert duration_condition.get_duration() == duration

    def test_evaluate(self, duration_condition: DurationCondition, duration: Duration):
        time: float = 3.0
        assert duration_condition.evaluate(state_vector=[], time=time) == (
            time - duration.in_seconds()
        )
