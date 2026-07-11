# Apache License 2.0

"""
CZML-based CesiumJS viewer for Open Space Toolkit objects.

The `Viewer` renders orbits, flight profiles, sensors, target vectors, ground tracks and
access windows by generating a CZML document (https://github.com/AnalyticalGraphicsInc/czml-writer/wiki)
displayed with CesiumJS, following the approach of Orekit's Oresium/OreCzml project
(https://gitlab.orekit.org/orekit/oresium).

The CZML document can also be exported with `Viewer.to_czml()` / `Viewer.save_czml()` for
use in any external CesiumJS application.
"""

from __future__ import annotations

import re
import uuid
import warnings
from dataclasses import dataclass

import numpy as np

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics.environment.object import Celestial
from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import Instant, Interval, Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics import Access
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State

from . import czml
from .utilities import lla_from_position
from .utilities import lla_from_state
from .utilities import position_from_lla

DEFAULT_SATELLITE_IMAGE: str = (
    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAADJSURBVDhPnZHRDcMgEEMZjVEYpaNklIzSEfLfD4qNnXAJSFWfhO7w2Zc0Tf9QG2rXrEzSUeZLOGm47WoH95x3Hl3jEgilvDgsOQUTqsNl68ezEwn1vae6lceSEEYvvWNT/Rxc4CXQNGadho1NXoJ+9iaqc2xi2xbt23PJCDIB6TQjOC6Bho/sDy3fBQT8PrVhibU7yBFcEPaRxOoeTwbwByCOYf9VGp1BYI1BA+EeHhmfzKbBoJEQwn1yzUZtyspIQUha85MpkNIXB7GizqDEECsAAAAASUVORK5CYII="
)
DEFAULT_STEP_DURATION: Duration = Duration.seconds(10.0)
DEFAULT_CESIUM_VERSION: str = "1.126"

EARTH_GRAVITATIONAL_PARAMETER_M3_S2: float = 3.986004418e14
EARTH_EQUATORIAL_RADIUS_M: float = 6378137.0
EARTH_POLAR_RADIUS_M: float = 6356752.314245179

_SCENE_MODES: dict[str, str] = {
    "2D": "Cesium.SceneMode.SCENE2D",
    "3D": "Cesium.SceneMode.SCENE3D",
    "COLUMBUS": "Cesium.SceneMode.COLUMBUS_VIEW",
}


@dataclass
class Sensor:
    name: str
    direction: tuple[float, float, float] | np.ndarray
    color: str


@dataclass
class ConicSensor(Sensor):
    half_angle: Angle
    length: Length


@dataclass
class RectangularSensor(Sensor):
    x_half_angle: Angle
    y_half_angle: Angle
    radius: Length


class Viewer:
    """
    A CesiumJS viewer, built by aggregating OSTk objects into a CZML document.
    """

    def __init__(
        self,
        interval: Interval,
        cesium_token: str | None = None,
        width: str = "1500px",
        height: str = "800px",
        zoom_to_entity: bool = True,
        track_entity: bool = True,
        scene_mode: str = "3D",
        scene_mode_picker: bool = True,
        clock_multiplier: float = 1.0,
        name: str = "OSTk Scenario",
    ) -> None:
        """
        Construct a Viewer.

        Args:
            interval (Interval): The time interval of the scenario.
            cesium_token (str, optional): The Cesium Ion access token (required for Ion imagery
                and Ion 3D model assets). Defaults to None, in which case OpenStreetMap imagery is used.
            width (str, optional): The CSS width of the viewer. Defaults to "1500px".
            height (str, optional): The CSS height of the viewer. Defaults to "800px".
            zoom_to_entity (bool, optional): Whether to zoom to the loaded scenario. Defaults to True.
            track_entity (bool, optional): Whether to track the first satellite entity. Defaults to True.
            scene_mode (str, optional): The initial scene mode: "3D", "2D" or "COLUMBUS". Defaults to "3D".
            scene_mode_picker (bool, optional): Whether to show the 2D/3D scene mode picker. Defaults to True.
            clock_multiplier (float, optional): The initial clock multiplier. Defaults to 1.0.
            name (str, optional): The scenario name. Defaults to "OSTk Scenario".
        """

        if scene_mode not in _SCENE_MODES:
            raise ValueError(
                f"Scene mode [{scene_mode}] is not supported. Must be one of {list(_SCENE_MODES)}."
            )

        self._interval: Interval = interval
        self._cesium_token: str | None = cesium_token
        self._width: str = width
        self._height: str = height
        self._zoom_to_entity: bool = zoom_to_entity
        self._track_entity: bool = track_entity
        self._scene_mode: str = scene_mode
        self._scene_mode_picker: bool = scene_mode_picker

        self._document: czml.Document = czml.Document(
            interval=interval,
            name=name,
            clock_multiplier=clock_multiplier,
        )
        self._entity_ids: set[str] = set()
        self._tracked_entity_id: str | None = None

    @property
    def interval(self) -> Interval:
        return self._interval

    def add_orbit(
        self,
        orbit: Orbit,
        step: Duration,
        name: str = "Satellite",
        show_orbital_track: bool = False,
        color: str | None = None,
        image: str | None = None,
        trail_time: Duration | None = None,
        lead_time: Duration | None = None,
    ) -> Viewer:
        """
        Add Orbit to Viewer.

        Args:
            orbit (Orbit): Orbit to be added.
            step (Duration): Step between two consecutive states.
            name (str, optional): Name of the orbit. Defaults to "Satellite".
            show_orbital_track (bool, optional): Whether to show the orbital track. Defaults to False.
            color (str, optional): Color of the orbit. Defaults to None.
            image (str, optional): Logo to be added. Defaults to None.
            trail_time (Duration, optional): Duration of the orbital track trailing the satellite.
                Defaults to None (one orbital period).
            lead_time (Duration, optional): Duration of the orbital track leading the satellite.
                Defaults to None (one orbital period).

        Returns:
            Viewer: The Viewer.
        """

        instants: list[Instant] = self._interval.generate_grid(step)
        states: list[State] = [
            state.in_frame(Frame.ITRF()) for state in orbit.get_states_at(instants)
        ]

        entity_id: str = self._make_entity_id(name)

        packet: dict = {
            "id": entity_id,
            "name": name,
            "availability": czml.format_interval(self._interval),
            "position": _sampled_position(instants, states),
            "billboard": {
                "image": image or DEFAULT_SATELLITE_IMAGE,
            },
            "label": {
                "text": name,
                "scale": 1.0,
                "fillColor": czml.format_color(color, default="white"),
                "pixelOffset": {"cartesian2": [0.0, 20.0]},
            },
        }

        if show_orbital_track:
            packet["path"] = self._path_property(
                states=states,
                step=step,
                color=color,
                trail_time=trail_time,
                lead_time=lead_time,
            )

        self._document.add_packet(packet)

        if self._tracked_entity_id is None:
            self._tracked_entity_id = entity_id

        return self

    def add_profile(
        self,
        profile: Profile,
        step: Duration,
        show_orbital_track: bool = False,
        cesium_asset_id: int | None = None,
        sensors: list[Sensor] | None = None,
        show_xyz_axes: bool = False,
        name: str = "Satellite",
        color: str | None = None,
        trail_time: Duration | None = None,
        lead_time: Duration | None = None,
    ) -> Viewer:
        """
        Add Profile to Viewer.

        Args:
            profile (Profile): Profile to be added.
            step (Duration): Step between two consecutive states.
            show_orbital_track (bool, optional): Whether to show the orbital track. Defaults to False.
            cesium_asset_id (int, optional): The Cesium Ion asset ID of the satellite 3D model. Defaults to None.
            sensors (list[Sensor], optional): Sensors to be added to the asset. Defaults to None.
            show_xyz_axes (bool, optional): Whether to show the XYZ axes. Defaults to False.
            name (str, optional): Name of the satellite. Defaults to "Satellite".
            color (str, optional): Color of the orbital track. Defaults to None.
            trail_time (Duration, optional): Duration of the orbital track trailing the satellite.
                Defaults to None (one orbital period).
            lead_time (Duration, optional): Duration of the orbital track leading the satellite.
                Defaults to None (one orbital period).

        Returns:
            Viewer: The Viewer.
        """

        if cesium_asset_id is None:
            warnings.warn(
                "No Cesium asset ID provided: the satellite is rendered with the default billboard.",
                FutureWarning,
                stacklevel=2,
            )

        instants: list[Instant] = self._interval.generate_grid(step)
        states: list[State] = [
            state.in_frame(Frame.ITRF()) for state in profile.get_states_at(instants)
        ]

        entity_id: str = self._make_entity_id(name)

        packet: dict = {
            "id": entity_id,
            "name": name,
            "availability": czml.format_interval(self._interval),
            "position": _sampled_position(instants, states),
            "orientation": czml.sampled_unit_quaternion(
                instants=instants,
                quaternions=[_cesium_quaternion_from_state(state) for state in states],
            ),
            "label": {
                "text": name,
                "scale": 1.0,
                "fillColor": czml.format_color(color, default="white"),
                "pixelOffset": {"cartesian2": [0.0, 20.0]},
            },
        }

        if cesium_asset_id is not None:
            # Ion asset URIs cannot be referenced from pure CZML:
            # the HTML template resolves this custom property into `entity.model` at load time.
            packet["properties"] = {"ion_asset_id": cesium_asset_id}
        else:
            packet["billboard"] = {"image": DEFAULT_SATELLITE_IMAGE}

        if show_orbital_track:
            packet["path"] = self._path_property(
                states=states,
                step=step,
                color=color,
                trail_time=trail_time,
                lead_time=lead_time,
            )

        self._document.add_packet(packet)

        if self._tracked_entity_id is None:
            self._tracked_entity_id = entity_id

        sensors = list(sensors or [])
        if show_xyz_axes:
            sensors.extend(
                [
                    ConicSensor(
                        name="x_axis",
                        direction=(+1.0, 0.0, 0.0),
                        half_angle=Angle.degrees(1.0),
                        length=Length.meters(2.0),
                        color="red",
                    ),
                    ConicSensor(
                        name="y_axis",
                        direction=(0.0, +1.0, 0.0),
                        half_angle=Angle.degrees(1.0),
                        length=Length.meters(2.0),
                        color="green",
                    ),
                    ConicSensor(
                        name="z_axis",
                        direction=(0.0, 0.0, +1.0),
                        half_angle=Angle.degrees(1.0),
                        length=Length.meters(2.0),
                        color="blue",
                    ),
                ]
            )

        for sensor in sensors:
            self._add_sensor(
                satellite_entity_id=entity_id,
                sensor=sensor,
                instants=instants,
                states=states,
            )

        return self

    def add_sensor_projection(
        self,
        profile: Profile,
        sensor: Sensor,
        time_step: Duration | None = None,
        color: str | None = None,
        name: str | None = None,
    ) -> Viewer:
        """
        Add the projection (footprint) of a sensor field of view onto the Earth ellipsoid.

        The sensor boundary rays are cast from the satellite along the (attitude-dependent)
        sensor directions, and their intersections with the WGS84 ellipsoid are displayed as a
        time-varying polygon. Time steps at which the sensor does not (fully) intersect the
        Earth are hidden.

        Args:
            profile (Profile): The satellite profile (position + attitude).
            sensor (Sensor): The sensor whose field of view is projected.
            time_step (Duration, optional): The duration of each step in the grid.
                Defaults to None. If None, the default step duration is used.
            color (str, optional): The color of the footprint. Defaults to None (sensor color).
            name (str, optional): The name of the footprint entity. Defaults to None.

        Returns:
            Viewer: The Viewer.
        """

        time_step = time_step or DEFAULT_STEP_DURATION

        instants: list[Instant] = self._interval.generate_grid(time_step)
        states: list[State] = [
            state.in_frame(Frame.ITRF()) for state in profile.get_states_at(instants)
        ]

        boundary_directions: list[np.ndarray] = _sensor_boundary_directions(sensor)

        positions_entries: list[dict] = []
        shown_intervals: list[Interval] = []

        for index in range(len(instants) - 1):
            state: State = states[index]
            satellite_position: np.ndarray = np.array(
                state.get_position().get_coordinates(), dtype=float
            )
            attitude: Quaternion = state.get_attitude()

            footprint: list[float] = []
            for direction_body in boundary_directions:
                direction_ecef: np.ndarray = np.array(
                    attitude.to_conjugate().rotate_vector(direction_body), dtype=float
                )
                intersection: np.ndarray | None = _intersection_with_earth(
                    origin=satellite_position,
                    direction=direction_ecef,
                )
                if intersection is None:
                    footprint = []
                    break
                footprint.extend(float(component) for component in intersection)

            if not footprint:
                continue

            interval: Interval = Interval.closed(
                start_instant=instants[index],
                end_instant=instants[index + 1],
            )
            shown_intervals.append(interval)
            positions_entries.append(
                {
                    "interval": czml.format_interval(interval),
                    "cartesian": footprint,
                }
            )

        entity_id: str = self._make_entity_id(name or f"{sensor.name}_projection")

        self._document.add_packet(
            {
                "id": entity_id,
                "name": name or f"{sensor.name} projection",
                "availability": czml.format_interval(self._interval),
                "polygon": {
                    "positions": positions_entries,
                    "material": {
                        "solidColor": {
                            "color": czml.format_color(
                                color or sensor.color, default="red", alpha=0.3
                            ),
                        },
                    },
                    "outline": True,
                    "outlineColor": czml.format_color(
                        color or sensor.color, default="red"
                    ),
                    "show": czml.interval_booleans(self._interval, shown_intervals),
                },
            }
        )

        return self

    def add_target_vector(
        self,
        profile_or_trajectory: Profile | Trajectory,
        target: Celestial | Trajectory | Profile | Position,
        name: str,
        color: str | None = None,
        time_step: Duration | None = None,
        length: Length | None = None,
    ) -> Viewer:
        """
        Add a time-varying vector (line) from a moving observer towards a target.

        The target can be a celestial body (Sun, Moon, ...), another trajectory/profile
        (e.g. another satellite), or a fixed position (e.g. a ground station).

        Args:
            profile_or_trajectory (Profile | Trajectory): The observer.
            target (Celestial | Trajectory | Profile | Position): The target.
            name (str): The name of the vector entity.
            color (str, optional): The color of the vector. Defaults to None.
            time_step (Duration, optional): The duration of each step in the grid.
                Defaults to None. If None, the default step duration is used.
            length (Length, optional): If provided, the vector is drawn with this fixed length
                along the target direction, instead of joining the target itself.

        Returns:
            Viewer: The Viewer.
        """

        time_step = time_step or DEFAULT_STEP_DURATION
        instants: list[Instant] = self._interval.generate_grid(time_step)

        observer_positions: list[np.ndarray] = _sampled_itrf_positions(
            profile_or_trajectory, instants
        )
        target_positions: list[np.ndarray] = _sampled_itrf_positions(target, instants)

        if length is not None:
            length_meters: float = float(length.in_meters())
            end_positions: list[np.ndarray] = []
            for observer_position, target_position in zip(
                observer_positions, target_positions
            ):
                direction: np.ndarray = target_position - observer_position
                end_positions.append(
                    observer_position
                    + direction / np.linalg.norm(direction) * length_meters
                )
        else:
            end_positions = target_positions

        start_id: str = self._make_entity_id(f"{name}_start")
        end_id: str = self._make_entity_id(f"{name}_end")
        entity_id: str = self._make_entity_id(name)

        self._document.add_packet(
            {
                "id": start_id,
                "availability": czml.format_interval(self._interval),
                "position": czml.sampled_cartesian(instants, observer_positions),
            }
        )
        self._document.add_packet(
            {
                "id": end_id,
                "availability": czml.format_interval(self._interval),
                "position": czml.sampled_cartesian(instants, end_positions),
            }
        )
        self._document.add_packet(
            {
                "id": entity_id,
                "name": name,
                "availability": czml.format_interval(self._interval),
                "polyline": {
                    "positions": {
                        "references": [f"{start_id}#position", f"{end_id}#position"],
                    },
                    "arcType": "NONE",
                    "width": 2,
                    "material": {
                        "solidColor": {
                            "color": czml.format_color(color, default="red"),
                        },
                    },
                },
            }
        )

        return self

    def add_celestial_body_direction(
        self,
        profile_or_trajectory: Profile | Trajectory,
        celestial: Celestial,
        time_step: Duration | None = None,
        color: str | None = None,
        length: Length | None = None,
    ) -> Viewer:
        """
        Add the direction from a moving observer towards a celestial body (Sun, Moon, Earth, ...).

        Args:
            profile_or_trajectory (Profile | Trajectory): The profile or trajectory to be added.
            celestial (Celestial): The celestial body to be used.
            time_step (Duration, optional): The duration of each step in the grid.
                Defaults to None. If None, the default step duration is used.
            color (str, optional): The color of the celestial body direction.
                Defaults to None. If None, the color depends on the celestial body
                (for the Earth, Sun and Moon). Otherwise, use the default color (RED).
            length (Length, optional): If provided, the direction is drawn with this fixed
                length instead of joining the celestial body itself.

        Returns:
            Viewer: The Viewer.
        """

        celestial_name: str = str(celestial.access_name())

        if color is None:
            color = {
                "Earth": "blue",
                "Moon": "grey",
                "Sun": "yellow",
            }.get(celestial_name, "red")

        return self.add_target_vector(
            profile_or_trajectory=profile_or_trajectory,
            target=celestial,
            name=f"{celestial_name.lower()}_direction",
            color=color,
            time_step=time_step,
            length=length,
        )

    def add_accesses(
        self,
        accesses: list[Access] | list[Interval],
        observer: Profile | Trajectory | Position | Celestial,
        target: Profile | Trajectory | Position | Celestial,
        name: str = "Access",
        color: str | None = None,
        time_step: Duration | None = None,
        show_label: bool = True,
    ) -> Viewer:
        """
        Visualize access windows between an observer and a target.

        A line joining the two objects is displayed during each access window, along with an
        optional label summarizing the current window. Clicking the line (or label) opens a
        panel (Cesium InfoBox) listing all the provided access windows.

        Args:
            accesses (list[Access] | list[Interval]): The access windows (or bare intervals).
            observer (Profile | Trajectory | Position | Celestial): The observer (e.g. a satellite).
            target (Profile | Trajectory | Position | Celestial): The target (e.g. a ground station
                or another satellite).
            name (str, optional): The name of the access entity. Defaults to "Access".
            color (str, optional): The color of the access line. Defaults to None (lime).
            time_step (Duration, optional): The duration of each step in the grid.
                Defaults to None. If None, the default step duration is used.
            show_label (bool, optional): Whether to show a label with the current access window
                details. Defaults to True.

        Returns:
            Viewer: The Viewer.
        """

        time_step = time_step or DEFAULT_STEP_DURATION

        access_intervals: list[Interval] = [
            access.get_interval() if isinstance(access, Access) else access
            for access in accesses
        ]

        instants: list[Instant] = self._interval.generate_grid(time_step)

        observer_positions: list[np.ndarray] = _sampled_itrf_positions(observer, instants)
        target_positions: list[np.ndarray] = _sampled_itrf_positions(target, instants)

        observer_id: str = self._make_entity_id(f"{name}_observer")
        target_id: str = self._make_entity_id(f"{name}_target")
        entity_id: str = self._make_entity_id(name)

        self._document.add_packet(
            {
                "id": observer_id,
                "availability": czml.format_interval(self._interval),
                "position": czml.sampled_cartesian(instants, observer_positions),
            }
        )
        self._document.add_packet(
            {
                "id": target_id,
                "availability": czml.format_interval(self._interval),
                "position": czml.sampled_cartesian(instants, target_positions),
            }
        )

        description: str = _access_description_html(name, access_intervals)
        show_property = czml.interval_booleans(self._interval, access_intervals)

        self._document.add_packet(
            {
                "id": entity_id,
                "name": name,
                "availability": czml.format_interval(self._interval),
                "description": description,
                "polyline": {
                    "positions": {
                        "references": [
                            f"{observer_id}#position",
                            f"{target_id}#position",
                        ],
                    },
                    "arcType": "NONE",
                    "width": 2,
                    "material": {
                        "solidColor": {
                            "color": czml.format_color(color, default="lime"),
                        },
                    },
                    "show": show_property,
                },
            }
        )

        if show_label:
            text_entries: list[dict] = [
                {
                    "interval": czml.format_interval(access_interval),
                    "string": (
                        f"{name} #{index + 1}\n"
                        f"{czml.format_instant(access_interval.get_start())} → "
                        f"{czml.format_instant(access_interval.get_end())}\n"
                        f"duration: {_format_duration(access_interval.get_duration())}"
                    ),
                }
                for index, access_interval in enumerate(access_intervals)
            ]

            self._document.add_packet(
                {
                    "id": f"{entity_id}_label",
                    "name": f"{name} label",
                    "availability": czml.format_interval(self._interval),
                    "description": description,
                    "position": {"reference": f"{target_id}#position"},
                    "label": {
                        "text": text_entries,
                        "scale": 0.5,
                        "fillColor": czml.format_color(color, default="lime"),
                        "pixelOffset": {"cartesian2": [0.0, -40.0]},
                        "show": show_property,
                    },
                }
            )

        return self

    def add_ground_tracks(
        self,
        profile_or_trajectory: Profile | Trajectory,
        time_step: Duration | None = None,
        show_current_position: bool = True,
    ) -> Viewer:
        """
        Add ground tracks to the viewer.

        Args:
            profile_or_trajectory (Profile | Trajectory): The profile or trajectory to be added.
            time_step (Duration, optional): The duration of each step in the grid.
                Default to None. If None, the default step duration is used.
            show_current_position (bool, optional): Whether to show the current position as a point. Defaults to True.

        Returns:
            Viewer: The Viewer.
        """

        time_step = time_step or DEFAULT_STEP_DURATION

        instants: list[Instant] = self._interval.generate_grid(time_step)
        llas: list[LLA] = []
        ground_track_positions: list[Position] = []

        for state in profile_or_trajectory.get_states_at(instants):
            satellite_lla: LLA = lla_from_state(state)
            lla: LLA = LLA(
                latitude=satellite_lla.get_latitude(),
                longitude=satellite_lla.get_longitude(),
                altitude=Length.meters(0.0),
            )
            llas.append(lla)
            ground_track_positions.append(position_from_lla(lla))

        self.add_line(
            positions=ground_track_positions,
            size=1,
            color="gray",
        )

        if show_current_position:
            self.add_moving_point(
                instants=instants,
                llas=llas,
                color="darkorange",
            )

        return self

    def add_target(
        self,
        position: Position,
        size: int | None = None,
        color: str | None = None,
        label: str | None = None,
    ) -> Viewer:
        """
        Add target to Viewer.

        Args:
            position (Position): Target position.
            size (int, optional): Target size. Defaults to None.
            color (str, optional): Target color. Defaults to None.
            label (str, optional): Target label. Defaults to None.

        Returns:
            Viewer: The Viewer.
        """

        self._document.add_packet(
            {
                "id": self._make_entity_id(label or "target"),
                "name": label or "Target",
                "position": _static_position(position),
                "point": {
                    "pixelSize": size or 10,
                    "color": czml.format_color(color, default="yellow"),
                },
            }
        )

        if label:
            self.add_label(position, label, color=color, pixel_offset=(0.0, -14.0))

        return self

    def add_line(
        self,
        positions: list[Position],
        size: int | None = None,
        color: str | None = None,
    ) -> Viewer:
        """
        Add line to Viewer.

        Args:
            positions (list[Position]): Line positions.
            size (int, optional): Line size. Defaults to None.
            color (str, optional): Line color. Defaults to None.

        Returns:
            Viewer: The Viewer.
        """

        cartographic_degrees: list[float] = []
        for lla in map(lla_from_position, positions):
            cartographic_degrees.extend(
                [
                    float(lla.get_longitude().in_degrees()),
                    float(lla.get_latitude().in_degrees()),
                    10.0,
                ]
            )

        self._document.add_packet(
            {
                "id": self._make_entity_id("line"),
                "polyline": {
                    "positions": {"cartographicDegrees": cartographic_degrees},
                    "width": size or 1,
                    "material": {
                        "solidColor": {
                            "color": czml.format_color(color, default="yellow"),
                        },
                    },
                },
            }
        )

        return self

    def add_moving_point(
        self,
        instants: list[Instant],
        llas: list[LLA],
        color: str | None = None,
        size: int | None = None,
    ) -> Viewer:
        """
        Add a moving point to the Viewer.

        Args:
            instants (list[Instant]): The list of instants.
            llas (list[LLA]): The list of Longitude, Latitude, Altitude (LLA) coordinates.
            color (str, optional): The color of the point. Defaults to None. If None, the default color is used.
            size (int, optional): The size of the point. Defaults to None. If None, the default size is used.

        Returns:
            Viewer: The Viewer.
        """

        positions: list[np.ndarray] = [
            np.array(position_from_lla(lla).get_coordinates(), dtype=float)
            for lla in llas
        ]

        self._document.add_packet(
            {
                "id": self._make_entity_id("moving_point"),
                "availability": czml.format_interval(self._interval),
                "position": czml.sampled_cartesian(instants, positions),
                "point": {
                    "pixelSize": size or 5,
                    "color": czml.format_color(color, default="darkorange"),
                },
            }
        )

        return self

    def add_label(
        self,
        position: Position,
        text: str,
        size: int | None = None,
        color: str | None = None,
        pixel_offset: tuple[float, float] | None = None,
    ) -> Viewer:
        """
        Add label to Viewer.

        Args:
            position (Position): Label position.
            text (str): Label text.
            size (int, optional): Label scale (1 is the natural size). Defaults to None (1).
            color (str, optional): Label color. Defaults to None.
            pixel_offset (tuple[float, float], optional): Screen-space offset in pixels.
                Defaults to None.

        Returns:
            Viewer: The Viewer.
        """

        label_property: dict = {
            "text": text,
            "scale": size or 1.0,
            "fillColor": czml.format_color(color, default="white"),
        }

        if pixel_offset is not None:
            label_property["pixelOffset"] = {"cartesian2": list(pixel_offset)}

        self._document.add_packet(
            {
                "id": self._make_entity_id(f"{text}_label"),
                "position": _static_position(position),
                "label": label_property,
            }
        )

        return self

    def to_czml(self) -> list[dict]:
        """
        Return the scenario as a list of CZML packets.

        Returns:
            list[dict]: The CZML packets (including the document packet).
        """

        return self._document.to_list()

    def save_czml(self, path: str) -> None:
        """
        Save the scenario as a `.czml` file, usable in any CesiumJS application.

        Args:
            path (str): The output file path.
        """

        self._document.save(path)

    def render(self) -> str:
        """
        Render Viewer as HTML string.

        Returns:
            str: Rendered HTML string.
        """

        container_id: str = f"cesium-container-{uuid.uuid4().hex[:8]}"
        cesium_base_url: str = (
            f"https://cesium.com/downloads/cesiumjs/releases/{DEFAULT_CESIUM_VERSION}/Build/Cesium"
        )

        viewer_options: list[str] = [
            "animation: true",
            "baseLayerPicker: false",
            "fullscreenButton: false",
            "geocoder: false",
            "homeButton: false",
            "infoBox: true",
            "navigationHelpButton: false",
            "navigationInstructionsInitiallyVisible: false",
            f"sceneModePicker: {'true' if self._scene_mode_picker else 'false'}",
            f"sceneMode: {_SCENE_MODES[self._scene_mode]}",
            "selectionIndicator: false",
            "shouldAnimate: false",
            "timeline: true",
        ]

        imagery_statement: str = ""
        if self._cesium_token is None:
            # Without an Ion token, use the NaturalEarthII imagery shipped with the CesiumJS
            # distribution itself (same origin as Cesium.js, no external tile service).
            viewer_options.append("baseLayer: false")
            imagery_statement = (
                'widget.scene.globe.baseColor = Cesium.Color.fromCssColorString("#0B1E33");'
                "Cesium.TileMapServiceImageryProvider.fromUrl("
                'Cesium.buildModuleUrl("Assets/Textures/NaturalEarthII")'
                ").then(function (provider) { widget.imageryLayers.addImageryProvider(provider); });"
            )

        token_statement: str = (
            f'Cesium.Ion.defaultAccessToken = "{self._cesium_token}";'
            if self._cesium_token is not None
            else ""
        )

        zoom_statement: str = "widget.zoomTo(dataSource);" if self._zoom_to_entity else ""

        track_statement: str = ""
        if self._track_entity and self._tracked_entity_id is not None:
            track_statement = (
                f'var trackedEntity = dataSource.entities.getById("{self._tracked_entity_id}");'
                "if (trackedEntity) { widget.trackedEntity = trackedEntity; }"
            )

        return (
            '<meta charset="utf-8">\n'
            f'<link href="{cesium_base_url}/Widgets/widgets.css" rel="stylesheet">\n'
            f'<div id="{container_id}" style="width: {self._width}; height: {self._height};"></div>\n'
            "<script>\n"
            "function _ostkLoadCesium(callback) {\n"
            "  if (window.Cesium) { callback(); return; }\n"
            '  var script = document.createElement("script");\n'
            f'  script.src = "{cesium_base_url}/Cesium.js";\n'
            "  script.onload = callback;\n"
            "  document.head.appendChild(script);\n"
            "}\n"
            "_ostkLoadCesium(function () {\n"
            f"  {token_statement}\n"
            f'  var widget = new Cesium.Viewer("{container_id}", {{{", ".join(viewer_options)}}});\n'
            f"  {imagery_statement}\n"
            f"  var czml = {self._document.to_json()};\n"
            "  widget.dataSources.add(Cesium.CzmlDataSource.load(czml)).then(function (dataSource) {\n"
            f"    {zoom_statement}\n"
            f"    {track_statement}\n"
            "    dataSource.entities.values.forEach(function (entity) {\n"
            '      if (entity.properties && entity.properties.hasProperty("ion_asset_id")) {\n'
            "        Cesium.IonResource.fromAssetId(entity.properties.ion_asset_id.getValue()).then(function (resource) {\n"
            "          entity.model = new Cesium.ModelGraphics({uri: resource, minimumPixelSize: 128, maximumScale: 20000});\n"
            "        });\n"
            "      }\n"
            "    });\n"
            "  });\n"
            "});\n"
            "</script>"
        )

    def _repr_html_(self) -> str:
        return self.render()

    def _make_entity_id(self, name: str) -> str:
        entity_id: str = re.sub(r"[^a-z0-9_]+", "_", name.lower()).strip("_") or "entity"

        if entity_id in self._entity_ids:
            suffix: int = 2
            while f"{entity_id}_{suffix}" in self._entity_ids:
                suffix += 1
            entity_id = f"{entity_id}_{suffix}"

        self._entity_ids.add(entity_id)

        return entity_id

    def _path_property(
        self,
        states: list[State],
        step: Duration,
        color: str | None,
        trail_time: Duration | None,
        lead_time: Duration | None,
    ) -> dict:
        """
        Build a CZML `path` property with leading/trailing orbital track visualization.
        """

        default_time_seconds: float = (
            _orbital_period_seconds(states[0])
            or self._interval.get_duration().in_seconds()
        )

        trail_time_seconds: float = (
            float(trail_time.in_seconds())
            if trail_time is not None
            else default_time_seconds
        )
        lead_time_seconds: float = (
            float(lead_time.in_seconds())
            if lead_time is not None
            else default_time_seconds
        )

        return {
            "material": {
                "solidColor": {
                    "color": czml.format_color(color, default="white"),
                },
            },
            "width": 1,
            "resolution": float(step.in_seconds()),
            "leadTime": lead_time_seconds,
            "trailTime": trail_time_seconds,
        }

    def _add_sensor(
        self,
        satellite_entity_id: str,
        sensor: Sensor,
        instants: list[Instant],
        states: list[State],
    ) -> None:
        """
        Add a body-fixed sensor visualization attached to a satellite entity.

        Standard CZML has no sensor volume support (`agi_conicSensor` & co. require a CesiumJS
        plugin), so sensors are approximated with native CZML primitives:

        - `ConicSensor`: a cone (CZML cylinder) whose apex is attached to the satellite.
        - `RectangularSensor`: the four field-of-view edge polylines.
        """

        if isinstance(sensor, ConicSensor):
            length_meters: float = float(sensor.length.in_meters())
            half_angle_radians: float = float(sensor.half_angle.in_radians())
            direction_body: np.ndarray = _unit(np.asarray(sensor.direction, dtype=float))

            center_positions: list[np.ndarray] = []
            orientations: list[np.ndarray] = []

            for state in states:
                satellite_position: np.ndarray = np.array(
                    state.get_position().get_coordinates(), dtype=float
                )
                direction_ecef: np.ndarray = np.array(
                    state.get_attitude().to_conjugate().rotate_vector(direction_body),
                    dtype=float,
                )
                center_positions.append(
                    satellite_position + direction_ecef * (length_meters / 2.0)
                )
                # The CZML cylinder axis is the entity Z axis, with `topRadius` at +Z:
                # align +Z with the anti-boresight so that the apex sits on the satellite.
                orientations.append(_quaternion_aligning_z(-direction_ecef))

            self._document.add_packet(
                {
                    "id": self._make_entity_id(f"{satellite_entity_id}_{sensor.name}"),
                    "name": sensor.name,
                    "availability": czml.format_interval(self._interval),
                    "position": czml.sampled_cartesian(instants, center_positions),
                    "orientation": czml.sampled_unit_quaternion(instants, orientations),
                    "cylinder": {
                        "length": length_meters,
                        "topRadius": 0.0,
                        "bottomRadius": length_meters * np.tan(half_angle_radians),
                        "material": {
                            "solidColor": {
                                "color": czml.format_color(
                                    sensor.color, default="red", alpha=0.3
                                ),
                            },
                        },
                        "outline": False,
                    },
                }
            )

        elif isinstance(sensor, RectangularSensor):
            radius_meters: float = float(sensor.radius.in_meters())
            corner_directions: list[np.ndarray] = _rectangular_corner_directions(sensor)

            for corner_index, corner_direction in enumerate(corner_directions):
                corner_positions: list[np.ndarray] = []

                for state in states:
                    satellite_position = np.array(
                        state.get_position().get_coordinates(), dtype=float
                    )
                    direction_ecef = np.array(
                        state.get_attitude()
                        .to_conjugate()
                        .rotate_vector(corner_direction),
                        dtype=float,
                    )
                    corner_positions.append(
                        satellite_position + direction_ecef * radius_meters
                    )

                corner_id: str = self._make_entity_id(
                    f"{satellite_entity_id}_{sensor.name}_corner_{corner_index}"
                )

                self._document.add_packet(
                    {
                        "id": corner_id,
                        "availability": czml.format_interval(self._interval),
                        "position": czml.sampled_cartesian(instants, corner_positions),
                    }
                )
                self._document.add_packet(
                    {
                        "id": self._make_entity_id(
                            f"{satellite_entity_id}_{sensor.name}_edge_{corner_index}"
                        ),
                        "name": sensor.name,
                        "availability": czml.format_interval(self._interval),
                        "polyline": {
                            "positions": {
                                "references": [
                                    f"{satellite_entity_id}#position",
                                    f"{corner_id}#position",
                                ],
                            },
                            "arcType": "NONE",
                            "width": 1,
                            "material": {
                                "solidColor": {
                                    "color": czml.format_color(
                                        sensor.color, default="orange"
                                    ),
                                },
                            },
                        },
                    }
                )

        else:
            raise NotImplementedError(f"{type(sensor).__name__} is not supported yet.")


def _unit(vector: np.ndarray) -> np.ndarray:
    return vector / np.linalg.norm(vector)


def _sampled_position(instants: list[Instant], states: list[State]) -> dict:
    """
    Build a CZML sampled position property from a list of OSTk States (in ITRF).
    """

    return czml.sampled_cartesian(
        instants=instants,
        coordinates=[
            np.array(state.get_position().get_coordinates(), dtype=float)
            for state in states
        ],
    )


def _static_position(position: Position, instant: Instant | None = None) -> dict:
    """
    Build a CZML static position property from an OSTk Position.
    """

    lla: LLA = lla_from_position(position, instant)

    return {
        "cartographicDegrees": [
            float(lla.get_longitude().in_degrees()),
            float(lla.get_latitude().in_degrees()),
            float(lla.get_altitude().in_meters()),
        ],
    }


def _sampled_itrf_positions(
    obj: Profile | Trajectory | Position | Celestial,
    instants: list[Instant],
) -> list[np.ndarray]:
    """
    Sample the ITRF position (in meters) of an OSTk object at the provided instants.
    """

    itrf: Frame = Frame.ITRF()

    if isinstance(obj, Position):
        return [
            np.array(
                obj.in_frame(instant=instant, frame=itrf).get_coordinates(), dtype=float
            )
            for instant in instants
        ]

    if isinstance(obj, Celestial):
        return [
            np.array(
                obj.get_position_in(frame=itrf, instant=instant)
                .in_meters()
                .get_coordinates(),
                dtype=float,
            )
            for instant in instants
        ]

    return [
        np.array(state.get_position().get_coordinates(), dtype=float)
        for state in (state.in_frame(itrf) for state in obj.get_states_at(instants))
    ]


def _cesium_quaternion_from_state(state: State) -> np.ndarray:
    """
    Convert an OSTk State attitude into a Cesium orientation quaternion [x, y, z, w].

    OSTk attitudes are passive rotations mapping the reference frame into the body frame
    (q_B_REF): their raw components, interpreted with Cesium's active (Hamilton) convention,
    map the body frame into the Earth-fixed frame, which is what Cesium expects.
    """

    quaternion: Quaternion = state.get_attitude()

    return np.array(
        [
            float(quaternion.x()),
            float(quaternion.y()),
            float(quaternion.z()),
            float(quaternion.s()),
        ]
    )


def _quaternion_aligning_z(direction: np.ndarray) -> np.ndarray:
    """
    Build a quaternion [x, y, z, w] rotating the +Z axis onto the provided direction.
    """

    z_axis: np.ndarray = np.array([0.0, 0.0, 1.0])
    target: np.ndarray = _unit(direction)

    cosine: float = float(np.dot(z_axis, target))

    if cosine > 1.0 - 1e-12:
        return np.array([0.0, 0.0, 0.0, 1.0])

    if cosine < -1.0 + 1e-12:
        return np.array([1.0, 0.0, 0.0, 0.0])  # 180 degrees about X

    axis: np.ndarray = _unit(np.cross(z_axis, target))
    angle: float = float(np.arccos(cosine))

    return np.array([*(axis * np.sin(angle / 2.0)), np.cos(angle / 2.0)])


def _orbital_period_seconds(state: State) -> float | None:
    """
    Estimate the (Keplerian) orbital period from a state, using the vis-viva equation.
    """

    state_gcrf: State = state.in_frame(Frame.GCRF())
    position: np.ndarray = np.array(
        state_gcrf.get_position().get_coordinates(), dtype=float
    )
    velocity: np.ndarray = np.array(
        state_gcrf.get_velocity().get_coordinates(), dtype=float
    )

    inverse_semi_major_axis: float = (
        2.0 / float(np.linalg.norm(position))
        - float(np.dot(velocity, velocity)) / EARTH_GRAVITATIONAL_PARAMETER_M3_S2
    )

    if inverse_semi_major_axis <= 0.0:
        return None

    semi_major_axis: float = 1.0 / inverse_semi_major_axis

    return float(
        2.0 * np.pi * np.sqrt(semi_major_axis**3 / EARTH_GRAVITATIONAL_PARAMETER_M3_S2)
    )


def _intersection_with_earth(
    origin: np.ndarray,
    direction: np.ndarray,
) -> np.ndarray | None:
    """
    Compute the first intersection of a ray with the WGS84 ellipsoid, or None if the ray misses.
    """

    scale: np.ndarray = np.array(
        [
            1.0 / EARTH_EQUATORIAL_RADIUS_M,
            1.0 / EARTH_EQUATORIAL_RADIUS_M,
            1.0 / EARTH_POLAR_RADIUS_M,
        ]
    )

    scaled_origin: np.ndarray = origin * scale
    scaled_direction: np.ndarray = _unit(direction) * scale

    a: float = float(np.dot(scaled_direction, scaled_direction))
    b: float = 2.0 * float(np.dot(scaled_origin, scaled_direction))
    c: float = float(np.dot(scaled_origin, scaled_origin)) - 1.0

    discriminant: float = b * b - 4.0 * a * c

    if discriminant < 0.0:
        return None

    sqrt_discriminant: float = float(np.sqrt(discriminant))

    for t in ((-b - sqrt_discriminant) / (2.0 * a), (-b + sqrt_discriminant) / (2.0 * a)):
        if t > 0.0:
            return origin + _unit(direction) * t

    return None


def _orthonormal_basis(axis: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Build two unit vectors orthogonal to the provided axis (deterministically).
    """

    reference: np.ndarray = (
        np.array([1.0, 0.0, 0.0]) if abs(axis[0]) < 0.9 else np.array([0.0, 1.0, 0.0])
    )

    u: np.ndarray = _unit(np.cross(axis, reference))
    v: np.ndarray = _unit(np.cross(axis, u))

    return u, v


def _sensor_boundary_directions(
    sensor: Sensor,
    conic_sample_count: int = 32,
    rectangular_edge_sample_count: int = 4,
) -> list[np.ndarray]:
    """
    Sample the boundary of a sensor field of view as unit direction vectors in the body frame.
    """

    axis: np.ndarray = _unit(np.asarray(sensor.direction, dtype=float))

    if isinstance(sensor, ConicSensor):
        half_angle: float = float(sensor.half_angle.in_radians())
        u, v = _orthonormal_basis(axis)

        return [
            _unit(
                np.cos(half_angle) * axis
                + np.sin(half_angle) * (np.cos(theta) * u + np.sin(theta) * v)
            )
            for theta in np.linspace(0.0, 2.0 * np.pi, conic_sample_count, endpoint=False)
        ]

    if isinstance(sensor, RectangularSensor):
        tan_x: float = float(np.tan(float(sensor.x_half_angle.in_radians())))
        tan_y: float = float(np.tan(float(sensor.y_half_angle.in_radians())))
        u, v = _orthonormal_basis(axis)

        corners: list[tuple[float, float]] = [
            (+tan_x, +tan_y),
            (+tan_x, -tan_y),
            (-tan_x, -tan_y),
            (-tan_x, +tan_y),
        ]

        directions: list[np.ndarray] = []
        for corner_index in range(4):
            start: tuple[float, float] = corners[corner_index]
            end: tuple[float, float] = corners[(corner_index + 1) % 4]
            for fraction in np.linspace(
                0.0, 1.0, rectangular_edge_sample_count, endpoint=False
            ):
                x: float = start[0] + (end[0] - start[0]) * fraction
                y: float = start[1] + (end[1] - start[1]) * fraction
                directions.append(_unit(axis + x * u + y * v))

        return directions

    raise NotImplementedError(f"{type(sensor).__name__} is not supported yet.")


def _rectangular_corner_directions(sensor: RectangularSensor) -> list[np.ndarray]:
    """
    Compute the four corner directions of a rectangular sensor field of view (body frame).
    """

    axis: np.ndarray = _unit(np.asarray(sensor.direction, dtype=float))
    tan_x: float = float(np.tan(float(sensor.x_half_angle.in_radians())))
    tan_y: float = float(np.tan(float(sensor.y_half_angle.in_radians())))
    u, v = _orthonormal_basis(axis)

    return [
        _unit(axis + x * tan_x * u + y * tan_y * v)
        for x, y in ((+1, +1), (+1, -1), (-1, -1), (-1, +1))
    ]


def _format_duration(duration: Duration) -> str:
    """
    Format a duration as HH:MM:SS.
    """

    total_seconds: int = int(round(float(duration.in_seconds())))
    hours, remainder = divmod(total_seconds, 3600)
    minutes, seconds = divmod(remainder, 60)

    return f"{hours:02d}:{minutes:02d}:{seconds:02d}"


def _access_description_html(name: str, intervals: list[Interval]) -> str:
    """
    Build the HTML description (Cesium InfoBox panel) summarizing access windows.
    """

    rows: str = "".join(
        "<tr>"
        f"<td>{index + 1}</td>"
        f"<td>{czml.format_instant(interval.get_start())}</td>"
        f"<td>{czml.format_instant(interval.get_end())}</td>"
        f"<td>{_format_duration(interval.get_duration())}</td>"
        "</tr>"
        for index, interval in enumerate(intervals)
    )

    return (
        f"<h2>{name}</h2>"
        '<table class="cesium-infoBox-defaultTable">'
        "<tr><th>#</th><th>Start (UTC)</th><th>End (UTC)</th><th>Duration</th></tr>"
        f"{rows}"
        "</table>"
    )


def _compute_celestial_direction_from_state(
    state: State,
    celestial: Celestial,
    frame: Frame | None = None,
) -> np.ndarray:
    """
    Compute the direction of a celestial body from a state.

    Args:
        state (State): The state of the observer.
        celestial (Celestial): The celestial body.
        frame (Frame): The frame in which the celestial body is expressed.
            Defaults to None. If None, the GCRF frame is used.

    Returns:
        np.ndarray: The direction of the celestial body (in meters).
    """
    frame = frame or Frame.GCRF()
    return (
        celestial.get_position_in(
            frame=frame,
            instant=state.get_instant(),
        )
        .in_meters()
        .get_coordinates()
        - state.get_position()
        .in_frame(
            frame=frame,
            instant=state.get_instant(),
        )
        .in_meters()
        .get_coordinates()
    )


def _compute_celestial_angular_diameter_from_states(
    celestial: Celestial,
    states: list[State],
) -> np.ndarray:
    """
    Compute the angular diameter of a celestial body from the states of an observer.

    Args:
        celestial (Celestial): The celestial body.
        states (list[State]): The states of the observer.

    Returns:
        np.ndarray: The angular diameter of the celestial body (in degrees).

    Reference:
        https://en.wikipedia.org/wiki/Angular_diameter
    """
    celestial_radius_meters: float = float(celestial.get_equatorial_radius().in_meters())
    celestial_to_observer_meters: np.ndarray = np.zeros((3, len(states)))

    for i, state in enumerate(states):
        celestial_to_observer_meters[:, i] = (
            state.in_frame(celestial.access_frame())
            .get_position()
            .in_meters()
            .get_coordinates()
        )
    distances: np.ndarray = np.linalg.norm(celestial_to_observer_meters, axis=0)
    return np.rad2deg(2 * np.arcsin(celestial_radius_meters / distances))
