# Apache License 2.0

from __future__ import annotations

import functools
import operator
from dataclasses import dataclass

import numpy as np

try:
    import cesiumpy

except ImportError:
    ...

from ostk.mathematics.geometry.d3.transformations.rotations import Quaternion

from ostk.physics.units import Length
from ostk.physics.units import Angle
from ostk.physics.time import Instant, Interval, Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA

from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.flight.profile import State

from .converters import coerce_to_datetime
from .utilities import lla_from_position


@dataclass
class Sensor:
    name: str
    direction: tuple[float, float, float] | np.ndarray
    color: str


@dataclass
class ConicSensor(Sensor):
    half_angle: Angle
    length: Length


class Viewer:
    def __init__(
        self,
        interval: Interval,
        cesium_token: str | None = None,
        width: str = "1500px",
        height: str = "800px",
    ) -> None:
        self._interval: Interval = interval

        self._viewer: cesiumpy.Viewer = cesiumpy.Viewer(
            width=width,
            height=height,
            clock_view_model=cesiumpy.ClockViewModel(
                clock=cesiumpy.Clock(
                    start_time=coerce_to_datetime(interval.get_start()),
                    stop_time=coerce_to_datetime(interval.get_end()),
                    clock_range=cesiumpy.Clock.Range.CLAMPED,
                    can_animate=True,
                    should_animate=False,
                )
            ),
            fullscreen_button=False,
            home_button=False,
            info_box=False,
            timeline=True,
            navigation_help_button=False,
            navigation_instructions_initially_visible=False,
            scene_mode_picker=False,
            selection_indicator=False,
            scene3d_only=True,
            zoom_to_entity=True,
            track_entity=True,
            default_access_token=cesium_token,
        )

    @property
    def interval(self) -> Interval:
        return self._interval

    def add_profile(
        self,
        profile: Profile,
        step: Duration,
        show_orbital_track: bool = False,
        cesium_asset_id: int | None = None,
        sensors: list[Sensor] | None = None,
    ) -> None:
        """
        Add Profile to Viewer.

        Args:
            profile (Profile): Profile to be added.
            step (Duration): Step between two consecutive states.
            show_orbital_track (bool, optional): Whether to show the orbital track. Defaults to False.
            cesium_asset_id (int, optional): The Cesium asset ID. Defaults to None.
            sensors (list[Sensor], optional): Sensors to be added to the asset. Defaults to None.
        """

        instants: list[Instant] = self._interval.generate_grid(step)
        states: list[State] = profile.get_states_at(instants)
        llas: list[LLA] = _generate_llas(states)

        satellite = cesiumpy.Satellite(
            position=_generate_sampled_position(instants, llas),
            orientation=_generate_sampled_orientation(states),
            availability=cesiumpy.TimeIntervalCollection(
                intervals=[
                    cesiumpy.TimeInterval(
                        start=coerce_to_datetime(self._interval.get_start()),
                        stop=coerce_to_datetime(self._interval.get_end()),
                    ),
                ],
            ),
            model=cesiumpy.IonResource(
                asset_id=cesium_asset_id or 0
            ),  # TBM: Should be made more robust
            sensors=[_cesium_from_ostk_sensor(sensor) for sensor in sensors or []],
        )

        satellite.render(self._viewer)

        if show_orbital_track:
            self._viewer.entities.add(
                cesiumpy.Polyline(
                    positions=cesiumpy.entities.cartesian.Cartesian3Array(
                        functools.reduce(
                            operator.iconcat,
                            [
                                [
                                    float(lla.get_longitude().in_degrees()),
                                    float(lla.get_latitude().in_degrees()),
                                    float(lla.get_altitude().in_meters()),
                                ]
                                for lla in _generate_llas(states)
                            ],
                            [],
                        )
                    ),
                    width=1,
                )
            )

    def add_target(
        self,
        position: Position,
        size: int | None = None,
        color: str | None = None,
    ) -> None:
        """
        Add target to Viewer.

        Args:
            position (Position): Target position.
            size (int, optional): Target size. Defaults to None.
            color (str, optional): Target color. Defaults to None.
        """

        self._viewer.entities.add(
            cesiumpy.Point(
                position=_cesium_from_ostk_position(position),
                pixel_size=size or 10,
                color=color or cesiumpy.color.YELLOW,
            )
        )

    def render(self) -> str:
        """
        Render Viewer as HTML string.

        Returns:
            str: Rendered HTML string.
        """

        return self._viewer.to_html()

    def _repr_html_(self) -> str:
        return self.render()


def _generate_llas(states: list[State]) -> list[LLA]:
    return [
        lla_from_position(state.get_position(), state.get_instant()) for state in states
    ]


def _generate_sampled_position(
    instants: list[Instant],
    llas: list[LLA],
) -> cesiumpy.SampledPositionProperty:
    """
    Generate a sampled position property from a list of OSTk LLAs.

    Args:
        states (list[LLA]): A list of OSTk LLAs.

    Returns:
        cesiumpy.SampledPositionProperty: Sampled position property.
    """

    return cesiumpy.SampledPositionProperty(
        samples=[
            (
                coerce_to_datetime(instant),
                _cesium_from_ostk_lla(lla),
                None,
            )
            for instant, lla in zip(instants, llas)
        ],
    )


def _generate_sampled_orientation(states: list[State]) -> cesiumpy.SampledProperty:
    """
    Generate a sampled orientation property from a list of OSTk States.

    Args:
        states (list[State]): A list of OSTk States.

    Returns:
        cesiumpy.SampledProperty: Sampled orientation property.
    """

    return cesiumpy.SampledProperty(
        type=cesiumpy.Quaternion,
        samples=[
            (
                coerce_to_datetime(state.get_instant()),
                _cesium_from_ostk_quaternion(state.in_frame(Frame.ITRF()).get_attitude()),
                None,
            )
            for state in states
        ],
    )


def _cesium_from_ostk_position(
    position: Position,
    instant: Instant | None = None,
) -> cesiumpy.Cartesian3:
    """
    Convert OSTk Position into Cesium Cartesian3.

    Args:
        position (Position): Position to convert.
        instant (Instant, optional): Instant at which the position is valid (required if position is not expressed in ECEF).

    Returns:
        cesiumpy.Cartesian3: Converted position.
    """

    return _cesium_from_ostk_lla(lla_from_position(position, instant))


def _cesium_from_ostk_lla(
    lla: LLA,
) -> cesiumpy.Cartesian3:
    """
    Convert OSTk LLA into Cesium Cartesian3.

    Args:
        lla (LLA): LLA to convert.

    Returns:
        cesiumpy.Cartesian3: Converted LLA.
    """

    return cesiumpy.Cartesian3.fromDegrees(
        float(lla.get_longitude().in_degrees()),
        float(lla.get_latitude().in_degrees()),
        float(lla.get_altitude().in_meters()),
    )


def _cesium_from_ostk_quaternion(quaternion: Quaternion) -> cesiumpy.Quaternion:
    """
    Convert OSTk Quaternion into Cesium Quaternion.

    Args:
        quaternion (Quaternion): Quaternion to convert.

    Returns:
        cesiumpy.Quaternion: Converted quaternion.
    """

    return cesiumpy.Quaternion(
        float(quaternion.x()),
        float(quaternion.y()),
        float(quaternion.z()),
        float(quaternion.s()),
    )


def _cesium_from_ostk_sensor(sensor: Sensor) -> cesiumpy.ConicSensor:
    if not isinstance(sensor, ConicSensor):
        raise NotImplementedError("Only Conic Sensor is supported at the moment.")

    return cesiumpy.ConicSensor(
        name=sensor.name,
        direction=cesiumpy.Cartesian3(*sensor.direction),
        half_angle=float(sensor.half_angle.in_radians()),
        length=float(sensor.length.in_meters()),
        material=sensor.color or cesiumpy.color.RED,
    )
