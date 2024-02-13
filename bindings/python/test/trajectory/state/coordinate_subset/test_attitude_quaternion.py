# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state.coordinate_subset import AttitudeQuaternion


@pytest.fixture
def name() -> str:
    return "Attitude"


class TestAttitudeQuaternion:
    def test_constructor(self, name: str):
        assert AttitudeQuaternion(name) is not None

    def test_default(self):
        assert AttitudeQuaternion.default() is not None
