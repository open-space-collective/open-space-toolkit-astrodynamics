# Apache License 2.0

from typing import Callable

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Position, Velocity, Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics import EventCondition


@pytest.fixture
def name() -> str:
    return "MyEvent"


@pytest.fixture
def evaluator() -> Callable:
    return lambda state: 5.0


@pytest.fixture
def target_value() -> float:
    return 0.0


@pytest.fixture
def target(target_value: float) -> EventCondition.Target:
    return EventCondition.Target(target_value, EventCondition.Target.Type.Relative)


@pytest.fixture
def event_condition(
    name: str, evaluator: Callable, target: EventCondition.Target
) -> EventCondition:
    class MyEventCondition(EventCondition):
        def is_satisfied(
            self,
            current_state,
            previous_state,
        ):
            return (
                current_state.get_position().get_coordinates()[2] > 0.0
                and previous_state.get_position().get_coordinates()[2] < 0.0
            )

    return MyEventCondition(name, evaluator, target)


class TestEventCondition:
    def test_subclass(self, event_condition: EventCondition):
        assert event_condition is not None

    def test_get_name(self, event_condition: EventCondition, name: str):
        assert event_condition.get_name() == name

    def test_get_evaluator(self, event_condition: EventCondition):
        assert event_condition.get_evaluator() is not None

    def test_get_target(
        self, event_condition: EventCondition, target: EventCondition.Target
    ):
        assert event_condition.get_target() == target

    def test_update_target(
        self,
        event_condition: EventCondition,
    ):
        current_target_value_offset: float = event_condition.get_target().value_offset
        event_condition.update_target(
            State(
                Instant.J2000(),
                Position.meters([0.0, 0.0, 0.0], Frame.GCRF()),
                Velocity.meters_per_second([0.0, 0.0, 0.0], Frame.GCRF()),
            )
        )

        assert event_condition.get_target().value_offset != current_target_value_offset
