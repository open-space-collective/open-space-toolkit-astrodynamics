# Apache License 2.0

from datetime import datetime

from ostk.physics.time import Scale
from ostk.physics.time import Instant


def coerce_to_datetime(instant: Instant | datetime) -> datetime:
    if isinstance(instant, datetime):
        return instant

    return instant.get_date_time(Scale.UTC)
