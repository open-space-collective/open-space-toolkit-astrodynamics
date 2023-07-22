# Apache License 2.0

import pytest

from ostk.physics.units import Length, Derived
from ostk.physics.environment.gravitational import Earth

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import SemiMajorAxisCondition


@pytest.fixture
def sma() -> Length:
    return Length.meters(7000000.0)


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Earth.spherical.gravitational_parameter


@pytest.fixture
def criteria() -> EventCondition.Criteria:
    return EventCondition.Criteria.PositiveOnly


@pytest.fixture
def sma_condition(
    criteria: EventCondition.Criteria, sma: Length, gravitational_parameter: Derived
) -> SemiMajorAxisCondition:
    return SemiMajorAxisCondition(criteria, sma, gravitational_parameter)


class TestSemiMajorAxisCondition:
    def test_get_semi_major_axis(self, sma_condition: EventCondition, sma: Length):
        assert sma_condition.get_semi_major_axis() == sma

    def test_get_gravitational_parameter(
        self, sma_condition: EventCondition, gravitational_parameter: Derived
    ):
        assert sma_condition.get_gravitational_parameter() == gravitational_parameter

    def test_evaluate(self, sma_condition: EventCondition, sma: Length):
        state_vector: list = [
            717094.039086306,
            -6872433.2241124,
            46175.9696673281,
            -970.650826004612,
            -45.4598114773158,
            7529.82424886455,
        ]
        assert sma_condition.evaluate(state_vector=state_vector, time=0.0) == (
            6904757.8910061345 - sma.in_meters()
        )
