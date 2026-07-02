# Apache License 2.0

import pytest

from ostk.physics.environment.gravitational import Earth
from ostk.physics.unit import Derived, Length, Angle
from ostk.physics.coordinate import Frame

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import (
    BrouwerLyddaneMeanShortCondition,
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
def frame() -> Frame:
    return Frame.GCRF()


class TestBrouwerLyddaneMeanShortCondition:
    @pytest.mark.parametrize(
        "static_constructor,target,criterion",
        (
            (
                BrouwerLyddaneMeanShortCondition.semi_major_axis,
                EventCondition.Target(Length.meters(7e6)),
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.eccentricity,
                EventCondition.Target(0.1),
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.inclination,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.aop,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.raan,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.true_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.mean_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.eccentric_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                BrouwerLyddaneMeanShortCondition.argument_of_latitude,
                EventCondition.Target(Angle.degrees(0.0)),
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

    @pytest.mark.parametrize(
        "static_constructor,target_range",
        (
            (
                BrouwerLyddaneMeanShortCondition.inclination,
                (Angle.degrees(10.0), Angle.degrees(20.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.aop,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.raan,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.true_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.mean_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.eccentric_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                BrouwerLyddaneMeanShortCondition.argument_of_latitude,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
        ),
    )
    def test_static_constructors_within_range(
        self,
        static_constructor,
        target_range,
        frame: Frame,
        gravitational_parameter: Derived,
    ):
        condition = static_constructor(frame, target_range, gravitational_parameter)
        assert condition is not None
