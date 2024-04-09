# Apache License 2.0

from datetime import datetime, timedelta, timezone

import numpy as np

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame


def coerce_to_datetime(value: Instant | datetime | str) -> datetime:
    """
    Return datetime from value.

    Args:
        value (Instant | datetime | str): A value to coerce.

    Returns:
        datetime: The coerced datetime.
    """

    if isinstance(value, datetime):
        return value

    if isinstance(value, Instant):
        return value.get_date_time(Scale.UTC).replace(tzinfo=timezone.utc)

    if isinstance(value, str):
        return datetime.fromisoformat(value)

    raise TypeError("Argument must be a datetime, an Instant, or a str.")


def coerce_to_instant(value: Instant | datetime | str) -> Instant:
    """
    Return Instant from value.

    Args:
        value (Instant | datetime | str): A value to coerce.

    Returns:
        Instant: The coerced Instant.
    """

    if isinstance(value, Instant):
        return value

    if isinstance(value, datetime):
        return Instant.date_time(value.astimezone(tz=timezone.utc), Scale.UTC)

    if isinstance(value, str):
        return coerce_to_instant(coerce_to_datetime(value))

    raise TypeError("Argument must be a datetime, an Instant, or a str.")


def coerce_to_iso(value: Instant | datetime | str, timespec: str = "microseconds") -> str:
    """
    Return an ISO string from value.

    Args:
        value (Instant | datetime | str): A value to coerce.
        timespec (str): A time resolution. Defaults to "microseconds".

    Returns:
        str: The coerced ISO string.
    """

    if isinstance(value, str):
        return coerce_to_iso(coerce_to_datetime(value), timespec=timespec)

    if isinstance(value, datetime):
        return value.isoformat(timespec=timespec)

    if isinstance(value, Instant):
        return coerce_to_iso(coerce_to_datetime(value), timespec=timespec)

    raise TypeError("Argument must be a datetime, an Instant, or a str.")


def coerce_to_interval(
    value: Interval | tuple[Instant, Instant] | tuple[datetime, datetime],
) -> Interval:
    """
    Return Interval from value.

    Args:
        value (Interval | tuple[Instant, Instant] | tuple[datetime, datetime]): A value to coerce.

    Returns:
        Interval: The coerced Interval.
    """

    if isinstance(value, Interval):
        return value

    return Interval.closed(
        start_instant=coerce_to_instant(value[0]),
        end_instant=coerce_to_instant(value[1]),
    )


def coerce_to_duration(value: Duration | timedelta) -> Duration:
    """
    Return Duration from value.

    Args:
        value (Duration | datetime): A value to coerce.

    Returns:
        Duration: The coerced Duration.
    """

    if isinstance(value, Duration):
        return value

    return Duration.seconds(value.total_seconds())


def coerce_to_position(
    value: Position | tuple[float, float, float] | list[float] | np.ndarray,
    frame: Frame,
) -> Position:
    """
    Return Position from value.

    Args:
        value (Position | tuple[float, float, float] | list[float] | np.ndarray): A value to coerce.
        frame (Frame): A reference frame.

    Returns:
        Position: The coerced Position.
    """

    if isinstance(value, Position):
        return value

    return Position.meters(value, frame)


def coerce_to_velocity(
    value: Velocity | tuple[float, float, float] | list[float] | np.ndarray,
    frame: Frame,
) -> Velocity:
    """
    Return Velocity from value.

    Args:
        value (Velocity | tuple[float, float, float] | list[float] | np.ndarray): A value to coerce.
        frame (Frame): A reference frame.

    Returns:
        Velocity: The coerced Velocity.
    """

    if isinstance(value, Velocity):
        return value

    return Velocity.meters_per_second(value, frame)


def coerce_to_quaternion(
    value: Quaternion | tuple[float, float, float, float] | list[float] | np.ndarray
) -> Quaternion:
    """
    Return quaternion from value.

    Args:
        value (Quaternion | tuple[float, float, float, float] | list[float] | np.ndarray): A value to coerce.

    Returns:
        Quaternion: The coerced Quaternion.
    """

    if isinstance(value, Quaternion):
        return value

    return Quaternion(vector=value, format=Quaternion.Format.XYZS)
