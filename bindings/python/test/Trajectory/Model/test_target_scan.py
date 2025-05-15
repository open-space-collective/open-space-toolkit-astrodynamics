# Apache License 2.0

import pytest

from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.astrodynamics.trajectory.model import TargetScan
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def earth() -> Earth:
    return Earth.WGS84()


@pytest.fixture
def start_lla() -> LLA:
    return LLA.vector([0.0, 0.0, 0.0])


@pytest.fixture
def end_lla() -> LLA:
    return LLA.vector([0.0, 1.0, 0.0])


@pytest.fixture
def start_instant() -> Instant:
    return Instant.date_time(DateTime(2023, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def end_instant() -> Instant:
    return Instant.date_time(DateTime(2023, 1, 1, 0, 10, 0), Scale.UTC)


@pytest.fixture
def target_scan(
    earth: Earth,
    start_lla: LLA,
    end_lla: LLA,
    start_instant: Instant,
    end_instant: Instant,
) -> TargetScan:
    return TargetScan(
        start_lla=start_lla,
        end_lla=end_lla,
        start_instant=start_instant,
        end_instant=end_instant,
        celestial=earth,
    )


class TestTargetScan:
    def test_constructor(
        self,
        target_scan: TargetScan,
    ):
        assert target_scan.is_defined()

    def test_calculate_state_at(
        self,
        target_scan: TargetScan,
    ):
        instant: Instant = Instant.date_time(DateTime(2023, 1, 1, 0, 5, 0), Scale.UTC)
        state: State = target_scan.calculate_state_at(instant)
        assert state is not None

    def test_equality_operator(
        self,
        target_scan: TargetScan,
    ):
        assert target_scan == target_scan

    def test_inequality_operator(
        self,
        target_scan: TargetScan,
        start_lla: LLA,
        end_lla: LLA,
        start_instant: Instant,
        earth: Earth,
    ):
        target_scan2 = TargetScan(
            start_lla=start_lla,
            end_lla=end_lla,
            start_instant=start_instant,
            end_instant=Instant.undefined(),
            celestial=earth,
        )
        assert target_scan != target_scan2
