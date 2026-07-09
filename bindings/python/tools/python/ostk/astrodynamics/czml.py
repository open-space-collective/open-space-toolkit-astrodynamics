# Apache License 2.0

"""
Low-level CZML (Cesium Language) document generation utilities.

CZML is a JSON-based declarative format describing time-dynamic scenes for CesiumJS:
https://github.com/AnalyticalGraphicsInc/czml-writer/wiki/CZML-Guide

This module converts Open Space Toolkit types (Instant, Interval, Position, ...) into
CZML properties and packets, without any third-party dependency. It is used by
`ostk.astrodynamics.viewer.Viewer`, and can also be used directly to produce `.czml`
files consumable by any CesiumJS application.
"""

from __future__ import annotations

import json
from datetime import datetime

from ostk.physics.time import Instant
from ostk.physics.time import Interval

from .converters import coerce_to_datetime

__all__ = [
    "Document",
    "format_color",
    "format_instant",
    "format_interval",
    "sampled_cartesian",
    "sampled_unit_quaternion",
    "interval_booleans",
]

_COLOR_NAMES: dict[str, tuple[int, int, int]] = {
    "black": (0, 0, 0),
    "blue": (0, 0, 255),
    "cyan": (0, 255, 255),
    "darkorange": (255, 140, 0),
    "gray": (128, 128, 128),
    "grey": (128, 128, 128),
    "green": (0, 128, 0),
    "lime": (0, 255, 0),
    "magenta": (255, 0, 255),
    "orange": (255, 165, 0),
    "pink": (255, 192, 203),
    "purple": (128, 0, 128),
    "red": (255, 0, 0),
    "white": (255, 255, 255),
    "yellow": (255, 255, 0),
}


def format_instant(instant: Instant | datetime | str) -> str:
    """
    Format an instant as a CZML (ISO 8601 UTC) string.

    Args:
        instant (Instant | datetime | str): The instant to format.

    Returns:
        str: The ISO 8601 string (e.g. "2020-01-01T00:00:00.000000Z").
    """

    return coerce_to_datetime(instant).strftime("%Y-%m-%dT%H:%M:%S.%fZ")


def format_interval(interval: Interval) -> str:
    """
    Format an interval as a CZML (ISO 8601 UTC) interval string.

    Args:
        interval (Interval): The interval to format.

    Returns:
        str: The ISO 8601 interval string (e.g. "2020-01-01T00:00:00Z/2020-01-02T00:00:00Z").
    """

    return f"{format_instant(interval.get_start())}/{format_instant(interval.get_end())}"


def format_color(
    color: str | tuple | list | None,
    default: str | tuple | list = "white",
    alpha: float | None = None,
) -> dict:
    """
    Convert a color into a CZML color property.

    Args:
        color (str | tuple | list | None): A color name (e.g. "red"), a hex string
            (e.g. "#FF0000" or "#FF0000CC"), or an RGB(A) sequence with components in [0, 255].
        default (str | tuple | list, optional): The color to use when `color` is None.
        alpha (float, optional): An alpha override in [0.0, 1.0]. Defaults to None (keep alpha).

    Returns:
        dict: The CZML color property (e.g. {"rgba": [255, 0, 0, 255]}).
    """

    value = color if color is not None else default

    if isinstance(value, str):
        text: str = value.strip().lower()

        if text.startswith("#"):
            text = text[1:]
            if len(text) not in (6, 8):
                raise ValueError(f"Invalid hex color [{value}].")
            components: list[int] = [
                int(text[i : i + 2], 16) for i in range(0, len(text), 2)
            ]
            rgba: list[int] = components + [255] * (4 - len(components))

        elif text in _COLOR_NAMES:
            rgba = [*_COLOR_NAMES[text], 255]

        else:
            raise ValueError(f"Unsupported color [{value}].")

    else:
        components = [int(component) for component in value]
        if len(components) == 3:
            rgba = components + [255]
        elif len(components) == 4:
            rgba = components
        else:
            raise ValueError(f"Invalid color sequence [{value}].")

    if alpha is not None:
        rgba[3] = int(round(max(0.0, min(1.0, alpha)) * 255))

    return {"rgba": rgba}


def sampled_cartesian(
    instants: list[Instant],
    coordinates: list,
    interpolation_degree: int = 5,
) -> dict:
    """
    Build a time-sampled CZML cartesian property (e.g. a position in the FIXED frame).

    Args:
        instants (list[Instant]): The sample instants.
        coordinates (list): The sample cartesian coordinates, in meters, as a list of 3-element sequences.
        interpolation_degree (int, optional): The Lagrange interpolation degree. Defaults to 5.

    Returns:
        dict: The CZML sampled property.
    """

    epoch: Instant = instants[0]

    samples: list[float] = []
    for instant, coordinate in zip(instants, coordinates):
        samples.append(float((instant - epoch).in_seconds()))
        samples.extend(float(component) for component in coordinate)

    return {
        "epoch": format_instant(epoch),
        "cartesian": samples,
        "interpolationAlgorithm": "LAGRANGE",
        "interpolationDegree": min(interpolation_degree, max(1, len(instants) - 1)),
        "referenceFrame": "FIXED",
    }


def sampled_unit_quaternion(
    instants: list[Instant],
    quaternions: list,
) -> dict:
    """
    Build a time-sampled CZML unit quaternion property (e.g. an entity orientation).

    Args:
        instants (list[Instant]): The sample instants.
        quaternions (list): The sample quaternions, as a list of [x, y, z, w] sequences,
            mapping the entity body frame into the Earth-fixed frame.

    Returns:
        dict: The CZML sampled property.
    """

    epoch: Instant = instants[0]

    samples: list[float] = []
    for instant, quaternion in zip(instants, quaternions):
        samples.append(float((instant - epoch).in_seconds()))
        samples.extend(float(component) for component in quaternion)

    return {
        "epoch": format_instant(epoch),
        "unitQuaternion": samples,
        "interpolationAlgorithm": "LINEAR",
        "interpolationDegree": 1,
    }


def interval_booleans(
    interval: Interval,
    true_intervals: list[Interval],
) -> list[dict] | bool:
    """
    Build a CZML boolean property covering `interval`, true over `true_intervals` and false elsewhere.

    Args:
        interval (Interval): The overall interval to cover.
        true_intervals (list[Interval]): The intervals over which the property is true.

    Returns:
        list[dict] | bool: The CZML interval'd boolean property.
    """

    if not true_intervals:
        return False

    entries: list[dict] = [
        {
            "interval": format_interval(interval),
            "boolean": False,
        }
    ]

    for true_interval in true_intervals:
        entries.append(
            {
                "interval": format_interval(true_interval),
                "boolean": True,
            }
        )

    return entries


class Document:
    """
    A CZML document: a clock-bearing document packet followed by entity packets.
    """

    def __init__(
        self,
        interval: Interval,
        name: str = "OSTk Scenario",
        clock_multiplier: float = 1.0,
    ) -> None:
        """
        Construct a CZML Document.

        Args:
            interval (Interval): The scenario time interval.
            name (str, optional): The document name. Defaults to "OSTk Scenario".
            clock_multiplier (float, optional): The clock multiplier (simulated seconds per
                wall-clock second). Defaults to 1.0.
        """

        self._interval: Interval = interval
        self._name: str = name
        self._clock_multiplier: float = clock_multiplier
        self._packets: list[dict] = []

    @property
    def interval(self) -> Interval:
        return self._interval

    @property
    def packets(self) -> list[dict]:
        return self._packets

    def add_packet(self, packet: dict) -> "Document":
        """
        Append a CZML packet to the document.

        Args:
            packet (dict): The CZML packet.

        Returns:
            Document: The document.
        """

        self._packets.append(packet)

        return self

    def to_list(self) -> list[dict]:
        """
        Return the document as a list of CZML packets (including the document packet).

        Returns:
            list[dict]: The CZML packets.
        """

        document_packet: dict = {
            "id": "document",
            "name": self._name,
            "version": "1.0",
            "clock": {
                "interval": format_interval(self._interval),
                "currentTime": format_instant(self._interval.get_start()),
                "multiplier": self._clock_multiplier,
                "range": "CLAMPED",
                "step": "SYSTEM_CLOCK_MULTIPLIER",
            },
        }

        return [document_packet, *self._packets]

    def to_json(self, indent: int | None = None) -> str:
        """
        Serialize the document to a CZML (JSON) string.

        Args:
            indent (int, optional): The JSON indentation. Defaults to None (compact).

        Returns:
            str: The CZML string.
        """

        return json.dumps(self.to_list(), indent=indent)

    def save(self, path: str, indent: int | None = 2) -> None:
        """
        Write the document to a `.czml` file.

        Args:
            path (str): The output file path.
            indent (int, optional): The JSON indentation. Defaults to 2.
        """

        with open(path, "w") as file:
            file.write(self.to_json(indent=indent))
