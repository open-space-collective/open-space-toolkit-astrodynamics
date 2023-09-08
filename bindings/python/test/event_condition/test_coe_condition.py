# Apache License 2.0

import pytest

from ostk.physics.environment.gravitational import Earth
from ostk.physics.units import Derived, Length, Angle
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.event_condition import (
    COECondition,
    AngularCondition,
    RealCondition,
)


@pytest.fixture
def criterion() -> AngularCondition.Criterion:
    return AngularCondition.Criterion.AnyCrossing


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Earth.spherical.gravitational_parameter


@pytest.fixture
def target() -> float:
    return 7e6


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


class TestCOECondition:
    @pytest.mark.parametrize(
        "static_constructor,target,criterion",
        (
            (
                COECondition.semi_major_axis,
                Length.meters(7e6),
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.eccentricity,
                0.1,
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.inclination,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.aop,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.raan,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.true_anomaly,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.mean_anomaly,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                COECondition.eccentric_anomaly,
                Angle.degrees(0.0),
                AngularCondition.Criterion.PositiveCrossing,
            ),
        ),
    )
    def test_static_constructors(
        self,
        static_constructor,
        target,
        criterion,
        frame: Frame,
        gravitational_parameter: Derived,
    ):
        condition = static_constructor(criterion, frame, target, gravitational_parameter)
        assert condition is not None
