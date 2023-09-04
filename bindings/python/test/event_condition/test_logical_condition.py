# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.time import DateTime, Instant, Scale

from ostk.astrodynamics.event_condition import RealCondition, LogicalCondition
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def always_true() -> RealCondition:
    return RealCondition(
        "Always True Condition",
        RealCondition.Criterion.StrictlyPositive,
        lambda state: 1.0,
        0.0,
    )


@pytest.fixture
def always_false() -> RealCondition:
    return RealCondition(
        "Always False Condition",
        RealCondition.Criterion.StrictlyPositive,
        lambda state: -1.0,
        0.0,
    )


@pytest.fixture
def event_conditions(
    always_true: RealCondition, always_false: RealCondition
) -> list[RealCondition]:
    return [always_true, always_false]


@pytest.fixture
def logical_condition_type() -> LogicalCondition.Type:
    return LogicalCondition.Type.And


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


@pytest.fixture
def logical_condition(
    logical_condition_type: LogicalCondition.Type, event_conditions: list[RealCondition]
) -> LogicalCondition:
    return LogicalCondition("Logical", logical_condition_type, event_conditions)


class TestLogicalCondition:
    @pytest.mark.parametrize(
        "logical_condition_type",
        (
            LogicalCondition.Type.And,
            LogicalCondition.Type.Or,
        ),
    )
    def test_constructor(
        self,
        logical_condition_type: LogicalCondition.Type,
        event_conditions: list[RealCondition],
    ):
        assert (
            LogicalCondition(
                "Logical Condition", logical_condition_type, event_conditions
            )
            is not None
        )

    def test_get_event_conditions(
        self,
        logical_condition: LogicalCondition,
        event_conditions: list[RealCondition],
    ):
        assert logical_condition.get_event_conditions() == event_conditions

    def test_get_type(
        self,
        logical_condition: LogicalCondition,
        logical_condition_type: LogicalCondition.Type,
    ):
        assert logical_condition.get_type() == logical_condition_type

    @pytest.mark.parametrize(
        "logical_condition_type,expected_result",
        (
            (LogicalCondition.Type.And, False),
            (LogicalCondition.Type.Or, True),
        ),
    )
    def test_is_satisfied(
        self,
        logical_condition_type: LogicalCondition.Type,
        expected_result: bool,
        event_conditions: list[RealCondition],
        state: State,
    ):
        logical_condition: LogicalCondition = LogicalCondition(
            "Logical Condition", logical_condition_type, event_conditions
        )

        assert (
            logical_condition.is_satisfied(previous_state=state, current_state=state)
            == expected_result
        )
