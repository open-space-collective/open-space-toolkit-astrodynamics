# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.time import DateTime, Duration, Instant, Scale

from ostk.astrodynamics.event_condition import DurationCondition
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def duration() -> Duration:
    return Duration.seconds(60.0)


@pytest.fixture
def reference_instant() -> Instant:
    return Instant.date_time(DateTime(2023, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def criterion() -> DurationCondition.Criterion:
    return DurationCondition.Criterion.StrictlyPositive


@pytest.fixture
def duration_condition(
    criterion: DurationCondition.Criterion,
    duration: Duration,
    reference_instant: Instant,
) -> DurationCondition:
    return DurationCondition(criterion, duration, reference_instant)


def create_state(instant: Instant) -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters(
        [717094.039086306, -6872433.2241124, 46175.9696673281], frame
    )
    velocity: Velocity = Velocity.meters_per_second(
        [-970.650826004612, -45.4598114773158, 7529.82424886455], frame
    )
    return State(instant, position, velocity)


class TestDurationCondition:
    def test_constructor_with_reference_instant(
        self,
        criterion: DurationCondition.Criterion,
        duration: Duration,
        reference_instant: Instant,
    ):
        condition = DurationCondition(criterion, duration, reference_instant)
        assert condition is not None

    def test_constructor_without_reference_instant(
        self,
        criterion: DurationCondition.Criterion,
        duration: Duration,
    ):
        condition = DurationCondition(criterion, duration)
        assert condition is not None
        assert condition.get_reference_instant() == Instant.J2000()

    def test_get_duration(
        self, duration_condition: DurationCondition, duration: Duration
    ):
        assert duration_condition.get_duration() == duration

    def test_get_reference_instant(
        self, duration_condition: DurationCondition, reference_instant: Instant
    ):
        assert duration_condition.get_reference_instant() == reference_instant

    def test_evaluate_at_reference_instant(
        self, duration_condition: DurationCondition, reference_instant: Instant
    ):
        state = create_state(reference_instant)
        # At reference instant, elapsed time is 0, target is 60, so evaluation is 0 - 60 = -60
        assert duration_condition.evaluate(state) == -60.0

    def test_evaluate_after_target_duration(
        self, duration_condition: DurationCondition, reference_instant: Instant
    ):
        state = create_state(reference_instant + Duration.seconds(60.0))
        # After 60 seconds, elapsed time is 60, target is 60, so evaluation is 60 - 60 = 0
        assert duration_condition.evaluate(state) == 0.0

    def test_evaluate_after_target_duration_positive(
        self, duration_condition: DurationCondition, reference_instant: Instant
    ):
        state = create_state(reference_instant + Duration.seconds(120.0))
        # After 120 seconds, elapsed time is 120, target is 60, so evaluation is 120 - 60 = 60
        assert duration_condition.evaluate(state) == 60.0

    def test_update_target(
        self,
        criterion: DurationCondition.Criterion,
        duration: Duration,
        reference_instant: Instant,
    ):
        condition = DurationCondition(criterion, duration, reference_instant)

        new_reference_instant = reference_instant + Duration.hours(1)
        state_at_new_reference = create_state(new_reference_instant)

        condition.update_target(state_at_new_reference)

        assert condition.get_reference_instant() == new_reference_instant

        # After update, evaluation at the new reference instant should be -60
        assert condition.evaluate(state_at_new_reference) == -60.0

    def test_is_satisfied_positive_crossing(
        self,
        duration: Duration,
        reference_instant: Instant,
    ):
        condition = DurationCondition(
            DurationCondition.Criterion.PositiveCrossing, duration, reference_instant
        )

        before_state = create_state(reference_instant + Duration.seconds(59.0))
        after_state = create_state(reference_instant + Duration.seconds(61.0))

        assert condition.is_satisfied(after_state, before_state) is True
        assert condition.is_satisfied(before_state, after_state) is False
