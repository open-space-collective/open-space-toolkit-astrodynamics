# Apache License 2.0

from datetime import datetime, timedelta, timezone

import numpy as np

from ostk.mathematics.geometry.d3.transformations.rotations import Quaternion

from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame


def coerce_to_datetime(value: Instant | datetime) -> datetime:
    """
    Return datetime from value.

    Args:
        value (Instant | datetime): A value to coerce.

    Returns:
        datetime: The coerced datetime.
    """

    if isinstance(value, datetime):
        return value

    return value.get_date_time(Scale.UTC).replace(tzinfo=timezone.utc)


def coerce_to_instant(value: Instant | datetime) -> Instant:
    """
    Return Instant from value.

    Args:
        value (Instant | datetime): A value to coerce.

    Returns:
        Instant: The coerced Instant.
    """

    if isinstance(value, Instant):
        return value

    return Instant.date_time(value, Scale.UTC)


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
