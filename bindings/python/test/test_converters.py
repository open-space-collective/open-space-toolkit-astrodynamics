# Apache License 2.0

from datetime import datetime, timedelta, timezone

import numpy as np

from ostk.mathematics.geometry.d3.transformations.rotations import Quaternion

from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.converters import coerce_to_datetime
from ostk.astrodynamics.converters import coerce_to_instant
from ostk.astrodynamics.converters import coerce_to_interval
from ostk.astrodynamics.converters import coerce_to_duration
from ostk.astrodynamics.converters import coerce_to_position
from ostk.astrodynamics.converters import coerce_to_velocity
from ostk.astrodynamics.converters import coerce_to_quaternion


def test_coerce_to_datetime_success_instant():
    value = Instant.date_time(DateTime(2020, 1, 1), Scale.UTC)
    assert coerce_to_datetime(value) == datetime(2020, 1, 1, tzinfo=timezone.utc)


def test_coerce_to_datetime_success_datetime():
    value = datetime(2020, 1, 1, tzinfo=timezone.utc)
    assert coerce_to_datetime(value) == value


def test_coerce_to_instant_success_datetime():
    value = datetime(2020, 1, 1, tzinfo=timezone.utc)
    assert coerce_to_instant(value) == Instant.date_time(DateTime(2020, 1, 1), Scale.UTC)


def test_coerce_to_instant_success_instant():
    value = Instant.date_time(DateTime(2020, 1, 1), Scale.UTC)
    assert coerce_to_instant(value) == value


def test_coerce_to_interval_success_interval():
    value = Interval.closed(
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )
    assert coerce_to_interval(value) == value


def test_coerce_to_interval_success_tuple_instant():
    value = (
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )
    assert coerce_to_interval(value) == Interval.closed(
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )


def test_coerce_to_interval_success_list_instant():
    value = [
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    ]
    assert coerce_to_interval(value) == Interval.closed(
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )


def test_coerce_to_interval_success_tuple_datetime():
    value = (
        datetime(2020, 1, 1, tzinfo=timezone.utc),
        datetime(2020, 1, 2, tzinfo=timezone.utc),
    )
    assert coerce_to_interval(value) == Interval.closed(
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )


def test_coerce_to_interval_success_list_datetime():
    value = [
        datetime(2020, 1, 1, tzinfo=timezone.utc),
        datetime(2020, 1, 2, tzinfo=timezone.utc),
    ]
    assert coerce_to_interval(value) == Interval.closed(
        Instant.date_time(DateTime(2020, 1, 1), Scale.UTC),
        Instant.date_time(DateTime(2020, 1, 2), Scale.UTC),
    )


def test_coerce_to_duration_success_duration():
    value = Duration.seconds(1.0)
    assert coerce_to_duration(value) == value


def test_coerce_to_duration_success_timedelta():
    value = timedelta(seconds=1.0)
    assert coerce_to_duration(value) == Duration.seconds(1.0)


def test_coerce_to_position_success_position():
    value = Position.meters((1.0, 2.0, 3.0), Frame.GCRF())
    assert coerce_to_position(value, Frame.GCRF()) == value


def test_coerce_to_position_success_tuple_float():
    value = (1.0, 2.0, 3.0)
    assert coerce_to_position(value, Frame.GCRF()) == Position.meters(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_position_success_list_float():
    value = [1.0, 2.0, 3.0]
    assert coerce_to_position(value, Frame.GCRF()) == Position.meters(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_position_success_np_array():
    value = np.array((1.0, 2.0, 3.0))
    assert coerce_to_position(value, Frame.GCRF()) == Position.meters(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_velocity_success_velocity():
    value = Velocity.meters_per_second((1.0, 2.0, 3.0), Frame.GCRF())
    assert coerce_to_velocity(value, Frame.GCRF()) == value


def test_coerce_to_velocity_success_tuple_float():
    value = (1.0, 2.0, 3.0)
    assert coerce_to_velocity(value, Frame.GCRF()) == Velocity.meters_per_second(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_velocity_success_list_float():
    value = [1.0, 2.0, 3.0]
    assert coerce_to_velocity(value, Frame.GCRF()) == Velocity.meters_per_second(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_velocity_success_np_array():
    value = np.array((1.0, 2.0, 3.0))
    assert coerce_to_velocity(value, Frame.GCRF()) == Velocity.meters_per_second(
        (1.0, 2.0, 3.0), Frame.GCRF()
    )


def test_coerce_to_quaternion_success_quaternion():
    value = Quaternion.xyzs(1.0, 2.0, 3.0, 4.0)
    assert coerce_to_quaternion(value) == value


def test_coerce_to_quaternion_success_list_float():
    value = [1.0, 2.0, 3.0, 4.0]
    assert coerce_to_quaternion(value) == Quaternion.xyzs(1.0, 2.0, 3.0, 4.0)


def test_coerce_to_quaternion_success_np_array():
    value = np.array((1.0, 2.0, 3.0, 4.0))
    assert coerce_to_quaternion(value) == Quaternion.xyzs(1.0, 2.0, 3.0, 4.0)


def test_coerce_to_quaternion_success_tuple_float():
    value = (1.0, 2.0, 3.0, 4.0)
    assert coerce_to_quaternion(value) == Quaternion.xyzs(1.0, 2.0, 3.0, 4.0)
