# Apache License 2.0

import pytest

from ostk.physics.environment.gravitational import Earth
from ostk.physics.unit import Derived, Length, Angle
from ostk.physics.coordinate import Frame

from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.event_condition import (
    OrbitalElementCondition,
    AngularCondition,
    RealCondition,
)


@pytest.fixture(
    params=(
        OrbitalElementCondition.Theory.Osculating,
        OrbitalElementCondition.Theory.BrouwerLyddaneMeanShort,
        OrbitalElementCondition.Theory.BrouwerLyddaneMeanLong,
    )
)
def theory(request) -> OrbitalElementCondition.Theory:
    return request.param


@pytest.fixture
def criterion() -> AngularCondition.Criterion:
    return AngularCondition.Criterion.AnyCrossing


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Earth.spherical.gravitational_parameter


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


class TestOrbitalElementCondition:
    @pytest.mark.parametrize(
        "static_constructor,target,criterion",
        (
            (
                OrbitalElementCondition.semi_major_axis,
                EventCondition.Target(Length.meters(7e6)),
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.eccentricity,
                EventCondition.Target(0.1),
                RealCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.inclination,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.aop,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.raan,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.true_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.mean_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.eccentric_anomaly,
                EventCondition.Target(Angle.degrees(0.0)),
                AngularCondition.Criterion.PositiveCrossing,
            ),
            (
                OrbitalElementCondition.argument_of_latitude,
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
        theory: OrbitalElementCondition.Theory,
        frame: Frame,
        gravitational_parameter: Derived,
    ):
        condition = static_constructor(
            theory, criterion, frame, target, gravitational_parameter
        )
        assert condition is not None

    @pytest.mark.parametrize(
        "static_constructor,target_range",
        (
            (
                OrbitalElementCondition.inclination,
                (Angle.degrees(10.0), Angle.degrees(20.0)),
            ),
            (
                OrbitalElementCondition.aop,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                OrbitalElementCondition.raan,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                OrbitalElementCondition.true_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                OrbitalElementCondition.mean_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                OrbitalElementCondition.eccentric_anomaly,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
            (
                OrbitalElementCondition.argument_of_latitude,
                (Angle.degrees(0.0), Angle.degrees(90.0)),
            ),
        ),
    )
    def test_static_constructors_within_range(
        self,
        static_constructor,
        target_range,
        theory: OrbitalElementCondition.Theory,
        frame: Frame,
        gravitational_parameter: Derived,
    ):
        condition = static_constructor(
            theory, frame, target_range, gravitational_parameter
        )
        assert condition is not None
