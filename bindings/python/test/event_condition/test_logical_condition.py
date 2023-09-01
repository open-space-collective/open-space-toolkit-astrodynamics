# Apache License 2.0

import pytest

from ostk.astrodynamics.event_condition import RealCondition, LogicalCondition


@pytest.fixture
def event_condition() -> RealCondition:
    return RealCondition(
        "My Condition",
        RealCondition.Criterion.StrictlyNegative,
        lambda x, t: t,
        1.0,
    )


@pytest.fixture
def another_event_condition() -> RealCondition:
    return RealCondition(
        "Another Condition",
        RealCondition.Criterion.StrictlyPositive,
        lambda x, t: t,
        2.0,
    )


@pytest.fixture
def event_conditions(
    event_condition: RealCondition, another_event_condition: RealCondition
) -> list[RealCondition]:
    return [event_condition, another_event_condition]


@pytest.fixture
def logical_condition_type() -> LogicalCondition.Type:
    return LogicalCondition.Type.And


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
    ):
        logical_condition: LogicalCondition = LogicalCondition(
            "Logical Condition", logical_condition_type, event_conditions
        )

        assert (
            logical_condition.is_satisfied(
                previous_state_vector=[-1.0],
                previous_time=-1.0,
                current_state_vector=[1.0],
                current_time=10.0,
            )
            == expected_result
        )
