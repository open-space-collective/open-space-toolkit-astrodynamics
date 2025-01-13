# Apache License 2.0

import pytest

from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.unit import Angle
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.data.provider import compute_off_nadir_angles


@pytest.fixture
def target_position() -> Position:
    return Position.meters([0.0, 0.0, 0.0], Frame.GCRF())


@pytest.fixture
def state() -> State:
    instant = Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC)
    position = Position.meters([0.0, 0.0, 0.0], Frame.GCRF())
    velocity = Velocity.meters_per_second([0.0, 0.0, 0.0], Frame.GCRF())

    return State(instant=instant, position=position, velocity=velocity)


class TestOffNadirAngles:

    def test_compute_off_nadir_angles(
        self,
        state: State,
        target_position: Position,
    ):
        along_track: Angle
        cross_track: Angle
        off_nadir: Angle
        along_track, cross_track, off_nadir = compute_off_nadir_angles(
            state, target_position
        )

        assert along_track is not None
        assert cross_track is not None
        assert off_nadir is not None

    def test_compute_off_nadir_angles_undefined(
        self,
        state: State,
        target_position: Position,
    ):
        with pytest.raises(RuntimeError):
            compute_off_nadir_angles(State.undefined(), target_position)

        with pytest.raises(RuntimeError):
            compute_off_nadir_angles(state, Position.undefined())
