# Apache License 2.0

import pytest

from datetime import datetime, timedelta, timezone

import numpy as np

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

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
from ostk.astrodynamics.converters import coerce_to_iso
from ostk.astrodynamics.converters import coerce_to_interval
from ostk.astrodynamics.converters import coerce_to_duration
from ostk.astrodynamics.converters import coerce_to_position
from ostk.astrodynamics.converters import coerce_to_velocity
from ostk.astrodynamics.converters import coerce_to_quaternion


def test_coerce_to_datetime_success_instant():
    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC)
    assert coerce_to_datetime(value) == datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc
    )

    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.TAI)
    assert coerce_to_datetime(value) == datetime(
        2020, 1, 2, 3, 3, 28, 123456, tzinfo=timezone.utc
    )


def test_coerce_to_datetime_success_datetime():
    value = datetime(2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc)
    assert coerce_to_datetime(value) == value

    value = datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone(timedelta(seconds=3600))
    )
    assert coerce_to_datetime(value) == value


def test_coerce_to_datetime_success_iso():
    value = "2020-01-02T03:04:05.123456+00:00"
    assert coerce_to_datetime(value) == datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc
    )

    value = "2020-01-02T03:04:05+00:00"
    assert coerce_to_datetime(value) == datetime(2020, 1, 2, 3, 4, 5, tzinfo=timezone.utc)

    value = "2020-01-02T03:04:05.123456+01:00"
    assert coerce_to_datetime(value) == datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone(timedelta(seconds=3600))
    )

    value = "2020-01-02T03:04:05.123456Z"
    assert coerce_to_datetime(value) == datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc
    )

    value = "2020-01-02T03:04:05Z"
    assert coerce_to_datetime(value) == datetime(2020, 1, 2, 3, 4, 5, tzinfo=timezone.utc)


def test_coerce_to_datetime_failure():
    with pytest.raises(TypeError):
        coerce_to_datetime(False)

    with pytest.raises(Exception):
        coerce_to_datetime("some_ill_formed_iso")


def test_coerce_to_instant_success_datetime():
    value = datetime(2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc)
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC
    )

    value = datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone(timedelta(seconds=3600))
    )
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 2, 4, 5, 123, 456), Scale.UTC
    )


def test_coerce_to_instant_success_instant():
    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC)
    assert coerce_to_instant(value) == value

    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.TAI)
    assert coerce_to_instant(value) == value


def test_coerce_to_instant_success_iso():
    value = "2020-01-02T03:04:05.123456+00:00"
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC
    )

    value = "2020-01-02T03:04:05+00:00"
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 3, 4, 5), Scale.UTC
    )

    value = "2020-01-02T03:04:05.123456+01:00"
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 2, 4, 5, 123, 456), Scale.UTC
    )

    value = "2020-01-02T03:04:05.123456Z"
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC
    )

    value = "2020-01-02T03:04:05Z"
    assert coerce_to_instant(value) == Instant.date_time(
        DateTime(
            2020,
            1,
            2,
            3,
            4,
            5,
        ),
        Scale.UTC,
    )


def test_coerce_to_instant_failure():
    with pytest.raises(TypeError):
        coerce_to_instant(False)

    with pytest.raises(Exception):
        coerce_to_instant("some_ill_formed_iso")


def test_coerce_to_iso_success_datetime():
    value = datetime(2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc)
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123456+00:00"
    )

    value = datetime(
        2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone(timedelta(seconds=3600))
    )
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123456+01:00"
    )

    value = datetime(2020, 1, 2, 3, 4, 5, 123456, tzinfo=timezone.utc)
    assert (
        coerce_to_iso(value, timespec="milliseconds") == "2020-01-02T03:04:05.123+00:00"
    )

    value = datetime(2020, 1, 2, 3, 4, 5, 0, tzinfo=timezone.utc)
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.000000+00:00"
    )


def test_coerce_to_iso_success_instant():
    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC)
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123456+00:00"
    )

    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.TAI)
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:03:28.123456+00:00"
    )

    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123, 456), Scale.UTC)
    assert (
        coerce_to_iso(value, timespec="milliseconds") == "2020-01-02T03:04:05.123+00:00"
    )

    value = Instant.date_time(DateTime(2020, 1, 2, 3, 4, 5, 123), Scale.UTC)
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123000+00:00"
    )


def test_coerce_to_iso_success_iso():
    value = "2020-01-02T03:04:05.123456+00:00"
    assert coerce_to_iso(value, timespec="microseconds") == value

    value = "2020-01-02T03:04:05.123456+01:00"
    assert coerce_to_iso(value, timespec="microseconds") == value

    value = "2020-01-02T03:04:05.123456+00:00"
    assert (
        coerce_to_iso(value, timespec="milliseconds") == "2020-01-02T03:04:05.123+00:00"
    )

    value = "2020-01-02T03:04:05.123+00:00"
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123000+00:00"
    )

    value = "2020-01-02T03:04:05+00:00"
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.000000+00:00"
    )

    value = "2020-01-02T03:04:05.123456Z"
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123456+00:00"
    )

    value = "2020-01-02T03:04:05.123456Z"
    assert (
        coerce_to_iso(value, timespec="milliseconds") == "2020-01-02T03:04:05.123+00:00"
    )

    value = "2020-01-02T03:04:05.123Z"
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.123000+00:00"
    )

    value = "2020-01-02T03:04:05Z"
    assert (
        coerce_to_iso(value, timespec="microseconds")
        == "2020-01-02T03:04:05.000000+00:00"
    )


def test_coerce_to_iso_failure():
    with pytest.raises(TypeError):
        coerce_to_iso(False)

    with pytest.raises(Exception):
        coerce_to_iso("some_ill_formed_iso")


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
