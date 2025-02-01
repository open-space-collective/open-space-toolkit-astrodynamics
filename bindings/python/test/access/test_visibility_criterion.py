# Apache License 2.0

import pytest
import numpy as np

from ostk.mathematics.object import RealInterval

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.unit import Angle

from ostk.astrodynamics.access import VisibilityCriterion


@pytest.fixture()
def instant() -> Instant:
    return Instant.date_time(DateTime(2023, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def azimuth_interval() -> RealInterval:
    return RealInterval.closed(0.0, 90.0)


@pytest.fixture
def elevation_interval() -> RealInterval:
    return RealInterval.closed(0.0, 45.0)


@pytest.fixture
def range_interval() -> RealInterval:
    return RealInterval.closed(0.0, 1e7)


@pytest.fixture
def aer_mask() -> dict[float, float]:
    return {
        0.0: 0.0,
        90.0: 45.0,
    }


class TestVisibilityCriterion:
    def test_aer_interval_constructor(
        self,
        azimuth_interval: RealInterval,
        elevation_interval: RealInterval,
        range_interval: RealInterval,
    ):
        aer_interval = VisibilityCriterion.AERInterval(
            azimuth_interval,
            elevation_interval,
            range_interval,
        )
        assert aer_interval is not None
        assert isinstance(aer_interval, VisibilityCriterion.AERInterval)

    def test_aer_mask_constructor(
        self,
        aer_mask: dict[float, float],
        range_interval: RealInterval,
    ):
        aer_mask_criterion = VisibilityCriterion.AERMask(aer_mask, range_interval)
        assert aer_mask_criterion is not None
        assert isinstance(aer_mask_criterion, VisibilityCriterion.AERMask)

    def test_line_of_sight_constructor(
        self,
        environment: Environment,
    ):
        line_of_sight = VisibilityCriterion.LineOfSight(environment)
        assert line_of_sight is not None
        assert isinstance(line_of_sight, VisibilityCriterion.LineOfSight)

    def test_elevation_interval_constructor(
        self,
        elevation_interval: RealInterval,
    ):
        elevation_criterion = VisibilityCriterion.ElevationInterval(elevation_interval)
        assert elevation_criterion is not None
        assert isinstance(elevation_criterion, VisibilityCriterion.ElevationInterval)

    def test_visibility_criterion_from_aer_interval(
        self,
        azimuth_interval: RealInterval,
        elevation_interval: RealInterval,
        range_interval: RealInterval,
    ):
        criterion = VisibilityCriterion.from_aer_interval(
            azimuth_interval, elevation_interval, range_interval
        )
        assert criterion is not None
        assert isinstance(criterion, VisibilityCriterion)
        assert criterion.is_aer_interval()

    def test_visibility_criterion_from_aer_mask(
        self,
        aer_mask: dict[float, float],
        range_interval: RealInterval,
    ):
        criterion = VisibilityCriterion.from_aer_mask(aer_mask, range_interval)
        assert criterion is not None
        assert isinstance(criterion, VisibilityCriterion)
        assert criterion.is_aer_mask()

    def test_visibility_criterion_from_line_of_sight(
        self,
        environment: Environment,
    ):
        criterion = VisibilityCriterion.from_line_of_sight(environment)
        assert criterion is not None
        assert isinstance(criterion, VisibilityCriterion)
        assert criterion.is_line_of_sight()

    def test_visibility_criterion_from_elevation_interval(
        self, elevation_interval: RealInterval
    ):
        criterion = VisibilityCriterion.from_elevation_interval(elevation_interval)
        assert criterion is not None
        assert isinstance(criterion, VisibilityCriterion)
        assert criterion.is_elevation_interval()

    def test_aer_interval_is_satisfied(
        self,
        azimuth_interval: RealInterval,
        elevation_interval: RealInterval,
        range_interval: RealInterval,
    ):
        aer_interval = VisibilityCriterion.AERInterval(
            azimuth_interval, elevation_interval, range_interval
        )
        assert (
            aer_interval.is_satisfied(azimuth=np.pi / 4, elevation=np.pi / 8, range=5e6)
            is True
        )

        assert (
            aer_interval.is_satisfied(azimuth=np.pi, elevation=np.pi / 2, range=1e8)
            is False
        )

    def test_elevation_interval_is_satisfied(
        self,
        elevation_interval: RealInterval,
    ):
        elevation_criterion = VisibilityCriterion.ElevationInterval(elevation_interval)
        elevation_valid = np.pi / 8  # 22.5 degrees
        assert elevation_criterion.is_satisfied(elevation_valid) is True
        assert elevation_criterion.is_satisfied(Angle.radians(elevation_valid)) is True

        elevation_invalid = np.pi / 2  # 90 degrees
        assert elevation_criterion.is_satisfied(elevation_invalid) is False
        assert elevation_criterion.is_satisfied(Angle.radians(elevation_invalid)) is False

    def test_line_of_sight_is_satisfied(
        self,
        environment: Environment,
    ):
        line_of_sight = VisibilityCriterion.LineOfSight(environment)
        instant = Instant.now()

        from_position = np.array([7000e3, 0.0, 0.0])
        to_position = np.array([7005e3, 0.0, 0.0])

        assert line_of_sight.is_satisfied(instant, from_position, to_position) is True

    def test_visibility_criterion_type_checks(
        self,
        elevation_interval: RealInterval,
        environment: Environment,
    ):
        criterion = VisibilityCriterion.from_elevation_interval(elevation_interval)
        assert criterion.is_elevation_interval() is True
        assert criterion.is_aer_interval() is False
        assert criterion.is_line_of_sight() is False

        criterion = VisibilityCriterion.from_line_of_sight(environment)
        assert criterion.is_line_of_sight() is True

    def test_visibility_criterion_cast(
        self,
        azimuth_interval: RealInterval,
        elevation_interval: RealInterval,
        range_interval: RealInterval,
    ):
        criterion = VisibilityCriterion.from_aer_interval(
            azimuth_interval, elevation_interval, range_interval
        )
        aer_interval = criterion.as_aer_interval()
        assert aer_interval is not None
        assert isinstance(aer_interval, VisibilityCriterion.AERInterval)

        with pytest.raises(ValueError):
            criterion.as_line_of_sight()
