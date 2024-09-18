# Apache License 2.0

import re

from datetime import datetime, timezone

from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Scale


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
        if value.endswith("Z"):
            value = value.replace("Z", "+00:00")

        return datetime.fromisoformat(value)

    raise TypeError(
        f"Argument is of type [{type(value)}]. Must be a datetime, an Instant, or a str."
    )


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

    raise TypeError(
        f"Argument is of type [{type(value)}]. Must be a datetime, an Instant, or a str."
    )


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

    raise TypeError(
        f"Argument is of type [{type(value)}]. Must be a datetime, an Instant, or a str."
    )


def coerce_to_interval(
    value: (
        Interval
        | tuple[Instant, Instant]
        | tuple[datetime, datetime]
        | tuple[str, str]
        | str
    ),
) -> Interval:
    """
    Return Interval from value.

    Args:
        value (Interval | tuple[Instant, Instant] | tuple[datetime, datetime] | tuple[str, str]): A value to coerce.

    Returns:
        Interval: The coerced Interval.
    """

    if isinstance(value, Interval):
        return value

    if isinstance(value, str):
        regex = r"\[(.*) - (.*)\] \[UTC\]"
        matches = re.search(regex, value)

        if matches:
            return Interval.closed(
                start_instant=coerce_to_instant(matches.group(1)),
                end_instant=coerce_to_instant(matches.group(2)),
            )

        raise ValueError(f"String [{value}] does not match the expected format.")

    return Interval.closed(
        start_instant=coerce_to_instant(value[0]),
        end_instant=coerce_to_instant(value[1]),
    )
