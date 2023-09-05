# Apache License 2.0

import pytest

from ostk.physics.environment.gravitational import Earth
from ostk.physics.units import Derived, Length, Angle
from ostk.physics.time import Instant, DateTime, Scale
from ostk.physics.coordinate import Frame, Position, Velocity

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.event_condition import COECondition
from ostk.astrodynamics.trajectory.orbit.models.kepler import COE


@pytest.fixture
def criterion() -> COECondition.Criterion:
    return COECondition.Criterion.AnyCrossing


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
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def condition(
    criterion: COECondition.Criterion,
    element: COE.Element,
    frame: Frame,
    target: float,
    gravitational_parameter: Derived,
) -> COECondition:
    return COECondition(
        "Test COECondition", criterion, element, frame, target, gravitational_parameter
    )


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters(
        [717094.039086306, -6872433.2241124, 46175.9696673281], frame
    )
    velocity: Velocity = Velocity.meters_per_second(
        [-970.650826004612, -45.4598114773158, 7529.82424886455], frame
    )

    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


class TestCOECondition:
    def test_constructor(
        self,
        criterion: COECondition.Criterion,
        element: COE.Element,
        frame: Frame,
        target: float,
        gravitational_parameter: Derived,
    ):
        name = "Test COECondition"
        condition = COECondition(
            name, criterion, element, frame, target, gravitational_parameter
        )

        assert condition is not None

    def test_getters(
        self,
        condition: COECondition,
        element: COE.Element,
        frame: Frame,
        target: float,
        gravitational_parameter: Derived,
    ):
        assert condition.get_element() == element
        assert condition.get_target() == target
        assert condition.get_frame() == frame
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
        self,
        static_constructor,
        frame: Frame,
        target: float,
        criterion: COECondition.Criterion,
        gravitational_parameter: Derived,
        state: State,
    ):
        condition = static_constructor(criterion, frame, target, gravitational_parameter)
        assert condition is not None

        assert condition.evaluate(state) is not None

    def test_evaluate(self, condition: COECondition, state: State, target: float):
        assert condition.evaluate(state) == pytest.approx(
            6904757.8910061345 - target, abs=1e-9
        )
