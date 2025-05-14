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
def target_scan() -> TargetScan:
    earth: Earth = Earth.wgs84()
    start_lla: LLA = LLA.degrees(0.0, 0.0, 0.0)
    end_lla: LLA = LLA.degrees(0.0, 1.0, 0.0)
    start_instant: Instant = Instant.date_time(DateTime(2023, 1, 1, 0, 0, 0), Scale.UTC)
    end_instant: Instant = Instant.date_time(DateTime(2023, 1, 1, 0, 10, 0), Scale.UTC)

    return TargetScan(
        start_lla=start_lla,
        end_lla=end_lla,
        start_instant=start_instant,
        end_instant=end_instant,
        earth=earth,
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

    def test_clone(
        self,
        target_scan: TargetScan,
    ):
        cloned_target_scan = target_scan.clone()
        assert cloned_target_scan == target_scan

    def test_equality_operator(
        self,
        target_scan: TargetScan,
    ):
        assert target_scan == target_scan

    def test_inequality_operator(
        self,
        target_scan: TargetScan,
    ):
        target_scan2 = TargetScan.undefined()
        assert target_scan != target_scan2
