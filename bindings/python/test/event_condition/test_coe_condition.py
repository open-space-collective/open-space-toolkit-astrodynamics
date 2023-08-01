# Apache License 2.0

import pytest

from ostk.physics.environment.gravitational import Earth
from ostk.physics.units import Derived, Length, Angle

from ostk.astrodynamics.event_condition import COECondition
from ostk.astrodynamics.trajectory.orbit.models.kepler import COE


@pytest.fixture
def criteria() -> COECondition.Criteria:
    return COECondition.Criteria.AnyCrossing


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Earth.spherical.gravitational_parameter


@pytest.fixture
def element() -> COE.Element:
    return COE.Element.SemiMajorAxis


@pytest.fixture
def target() -> float:
    return 7e6


@pytest.fixture
def condition(
    criteria: COECondition.Criteria,
    element: COE.Element,
    target: float,
    gravitational_parameter: Derived,
) -> COECondition:
    return COECondition(
        "Test COECondition", criteria, element, target, gravitational_parameter
    )


@pytest.fixture
def state_vector() -> list[float]:
    return [
        717094.039086306,
        -6872433.2241124,
        46175.9696673281,
        -970.650826004612,
        -45.4598114773158,
        7529.82424886455,
    ]


class TestCOECondition:
    def test_constructor(self, criteria, element, target, gravitational_parameter):
        name = "Test COECondition"
        condition = COECondition(name, criteria, element, target, gravitational_parameter)

        assert condition is not None

    def test_getters(self, condition, element, target, gravitational_parameter):
        assert condition.get_element() == element
        assert condition.get_target() == target
        assert condition.get_gravitational_parameter() == gravitational_parameter

    @pytest.mark.parametrize(
        "static_constructor,target",
        (
            (COECondition.semi_major_axis, Length.meters(7e6)),
            (COECondition.eccentricity, 0.1),
            (COECondition.inclination, Angle.degrees(0.0)),
            (COECondition.aop, Angle.degrees(0.0)),
            (COECondition.raan, Angle.degrees(0.0)),
            (COECondition.true_anomaly, Angle.degrees(0.0)),
            (COECondition.mean_anomaly, Angle.degrees(0.0)),
            (COECondition.eccentric_anomaly, Angle.degrees(0.0)),
        ),
    )
    def test_static_constructors(
        self, static_constructor, target, criteria, gravitational_parameter
    ):
        assert static_constructor(criteria, target, gravitational_parameter) is not None

    def test_evaluate(self, condition, state_vector, target):
        assert condition.evaluate(state_vector, 0.0) == pytest.approx(
            6904757.8910061345 - target, abs=1e-9
        )

    def test_string_from_element(self):
        element_str = COECondition.string_from_element(COE.Element.SemiMajorAxis)
        assert element_str == "Semi-major axis"
