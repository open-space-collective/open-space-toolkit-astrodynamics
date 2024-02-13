# Apache License 2.0

import pytest

from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    AngularVelocity,
    AttitudeQuaternion,
)


@pytest.fixture
def attitude_quaternion() -> AttitudeQuaternion:
    return AttitudeQuaternion.default()


@pytest.fixture
def name() -> str:
    return "Angular Velocity"


class TestAngularVelocity:
    def test_constructor(
        self,
        attitude_quaternion: AttitudeQuaternion,
        name: str,
    ):
        assert AngularVelocity(attitude_quaternion, name) is not None

    def test_default(self):
        assert AttitudeQuaternion.default() is not None
