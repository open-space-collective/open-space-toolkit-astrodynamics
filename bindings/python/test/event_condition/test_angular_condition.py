# Apache License 2.0

import pytest

from ostk.physics.time import Instant, DateTime, Scale
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.units import Angle

from ostk.astrodynamics.trajectory import State

from ostk.astrodynamics.event_condition import AngularCondition


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
def criterion() -> AngularCondition.Criterion:
    return AngularCondition.Criterion.PositiveCrossing


@pytest.fixture
def evaluator():
    return lambda state: state.get_coordinates()[0]


@pytest.fixture
def target_angle() -> Angle:
    return Angle.degrees(0.0)


@pytest.fixture
def target_range() -> tuple[Angle, Angle]:
    return (Angle.degrees(0.0), Angle.degrees(10.0))


@pytest.fixture
def event_condition(
    criterion: AngularCondition.Criterion, evaluator, target_angle: Angle
) -> AngularCondition:
    return AngularCondition(
        "My Condition",
        criterion,
        evaluator,
        target_angle,
    )


@pytest.fixture
def within_range_condition(
    evaluator, target_range: tuple[Angle, Angle]
) -> AngularCondition:
    return AngularCondition.within_range("My Condition", evaluator, target_range)


class TestAngularCondition:
    def test_constructor(
        self,
        criterion: AngularCondition.Criterion,
        evaluator,
        target_angle: Angle,
    ):
        assert (
            AngularCondition("My Condition", criterion, evaluator, target_angle)
            is not None
        )

    def test_get_criterion(
        self,
        event_condition: AngularCondition,
        criterion: AngularCondition.Criterion,
    ):
        assert event_condition.get_criterion() == criterion

    def test_get_target_angle(
        self,
        event_condition: AngularCondition,
        within_range_condition: AngularCondition,
        target_angle: Angle,
    ):
        assert event_condition.get_target_angle() == target_angle

        with pytest.raises(RuntimeError):
            within_range_condition.get_target_angle()

    def test_get_target_range(
        self,
        event_condition: AngularCondition,
        within_range_condition: AngularCondition,
        target_range: tuple[Angle, Angle],
    ):
        with pytest.raises(RuntimeError):
            event_condition.get_target_range()

        assert within_range_condition.get_target_range() == target_range

    def test_is_satisfied(self, state: State, event_condition: AngularCondition):
        assert (
            event_condition.is_satisfied(previous_state=state, current_state=state)
            is not None
        )

    def test_string_from_criterion(self, criterion: AngularCondition.Criterion):
        assert AngularCondition.string_from_criterion(criterion) is not None
