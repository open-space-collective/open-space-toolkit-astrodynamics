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

from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

from ostk.physics.environment.object import Celestial
from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import Instant, Interval, Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State

from .converters import coerce_to_datetime
from .utilities import lla_from_position
from .utilities import lla_from_state
from .utilities import position_from_lla

DEFAULT_SATELLITE_IMAGE: str = (
    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAADJSURBVDhPnZHRDcMgEEMZjVEYpaNklIzSEfLfD4qNnXAJSFWfhO7w2Zc0Tf9QG2rXrEzSUeZLOGm47WoH95x3Hl3jEgilvDgsOQUTqsNl68ezEwn1vae6lceSEEYvvWNT/Rxc4CXQNGadho1NXoJ+9iaqc2xi2xbt23PJCDIB6TQjOC6Bho/sDy3fBQT8PrVhibU7yBFcEPaRxOoeTwbwByCOYf9VGp1BYI1BA+EeHhmfzKbBoJEQwn1yzUZtyspIQUha85MpkNIXB7GizqDEECsAAAAASUVORK5CYII="
)
DEFAULT_STEP_DURATION: Duration = Duration.seconds(10.0)


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
    def __init__(
        self,
        interval: Interval,
        cesium_token: str | None = None,
        width: str = "1500px",
        height: str = "800px",
        zoom_to_entity: bool = True,
        track_entity: bool = True,
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
            zoom_to_entity=zoom_to_entity,
            track_entity=track_entity,
            default_access_token=cesium_token,
        )

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

        Returns:
            Viewer: The Viewer.
        """
        instants: list[Instant] = self._interval.generate_grid(step)
        states: list[State] = orbit.get_states_at(instants)
        llas: list[LLA] = _generate_llas(states)

        cesium_positions: cesiumpy.SampledPositionProperty = (
            _generate_sampled_position_from_states(states)
        )

        self._viewer.entities.add(
            cesiumpy.Billboard(
                name=name,
                position=cesium_positions,
                image=image or DEFAULT_SATELLITE_IMAGE,
            )
        )

        self._viewer.entities.add(
            cesiumpy.Label(
                position=cesium_positions,
                text=name,
                scale=1.0,
                fill_color=color or cesiumpy.color.WHITE,
                pixel_offset=[0.0, 20.0],
            )
        )

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
                                for lla in llas
                            ],
                            [],
                        )
                    ),
                    width=1,
                )
            )

        return self

    def add_profile(
        self,
        profile: Profile,
        step: Duration,
        show_orbital_track: bool = False,
        cesium_asset_id: int | None = None,
        sensors: list[Sensor] | None = None,
        show_xyz_axes: bool = False,
    ) -> Viewer:
        """
        Add Profile to Viewer.

        Args:
            profile (Profile): Profile to be added.
            step (Duration): Step between two consecutive states.
            show_orbital_track (bool, optional): Whether to show the orbital track. Defaults to False.
            cesium_asset_id (int, optional): The Cesium asset ID. Defaults to None.
            sensors (list[Sensor], optional): Sensors to be added to the asset. Defaults to None.
            show_xyz_axes (bool, optional): Whether to show the XYZ axes. Defaults to False.

        Returns:
            Viewer: The Viewer.
        """

        instants: list[Instant] = self._interval.generate_grid(step)
        states: list[State] = profile.get_states_at(instants)
        llas: list[LLA] = _generate_llas(states)

        sensors = sensors or []
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

        satellite = cesiumpy.Satellite(
            position=_generate_sampled_position_from_llas(instants, llas),
            orientation=_generate_sampled_orientation(states),
            availability=self._get_availability(),
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

        return self

    def add_celestial_body_direction(
        self,
        profile_or_trajectory: Profile | Trajectory,
        celestial: Celestial,
        time_step: Duration | None = None,
        color: str | None = None,
    ) -> Viewer:
        """
        Add the celestial direction to the viewer.

        Args:
            profile_or_trajectory (Profile | Trajectory): The profile or trajectory to be added.
            celestial (Celestial, optional): The celestial body to be used.
            time_step (Duration): The duration of each step in the grid.
                Default to None. If None, the default step duration is used.
            color (str, optional): The color of the celestial body direction.
                Defaults to None. If None, the color depends on the celestial body (for the Earth, Sun and Moon).
                Otherwise, use the default color (RED).

        Returns:
            Viewer: The Viewer.
        """
        time_step = time_step or DEFAULT_STEP_DURATION
        alpha_color: float = 0.5
        reference_frame: Frame = Frame.GCRF()
        reference_vector: np.ndarray = np.array([0.0, 0.0, 1.0])
        instants: list[Instant] = self._interval.generate_grid(time_step)
        celestial_name: str = str(celestial.access_name())

        if color is None:
            if celestial_name == "Earth":
                color = cesiumpy.color.BLUE
            elif celestial_name == "Moon":
                color = cesiumpy.color.GREY
            elif celestial_name == "Sun":
                color = cesiumpy.color.YELLOW
            else:
                color = cesiumpy.color.RED

            # Apply an alpha to the color
            color = color.with_alpha(alpha_color)

        def _create_celestial_body_direction_state(
            satellite_state: State,
            reference_frame: Frame = reference_frame,
            reference_vector: np.ndarray = reference_vector,
            celestial: Celestial = celestial,
        ) -> State:
            state_in_reference_frame: State = satellite_state.in_frame(reference_frame)
            return State(
                instant=state_in_reference_frame.get_instant(),
                position=state_in_reference_frame.get_position(),
                velocity=state_in_reference_frame.get_velocity(),
                attitude=Quaternion.shortest_rotation(
                    first_vector=_compute_celestial_direction_from_state(
                        state=satellite_state,
                        celestial=celestial,
                        frame=reference_frame,
                    ),
                    second_vector=reference_vector,
                ),
                angular_velocity=np.zeros(3),
                attitude_frame=reference_frame,
            )

        celestial_direction_states: list[State] = list(
            map(
                _create_celestial_body_direction_state,
                profile_or_trajectory.get_states_at(instants),
            )
        )

        satellite = cesiumpy.Satellite(
            position=_generate_sampled_position_from_llas(
                instants=instants,
                llas=_generate_llas(celestial_direction_states),
            ),
            orientation=_generate_sampled_orientation(celestial_direction_states),
            availability=self._get_availability(),
        )

        _cesium_from_ostk_sensor(
            ConicSensor(
                name=celestial_name.lower() + "_direction",
                direction=reference_vector,
                # Compute the half angle from the celestial body diameter
                half_angle=Angle.degrees(
                    _compute_celestial_angular_diameter_from_states(
                        celestial=celestial,
                        states=celestial_direction_states,
                    ).mean()
                    / 2.0
                ),
                length=Length.meters(2.0),
                color=color,
            )
        ).render(
            viewer=self._viewer,
            satellite=satellite,
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
            color=cesiumpy.color.GRAY,
        )

        if show_current_position:
            self.add_moving_point(
                instants=instants,
                llas=llas,
                color=cesiumpy.color.DARKORANGE,
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

        self._viewer.entities.add(
            cesiumpy.Point(
                position=_cesium_from_ostk_position(position),
                pixel_size=size or 10,
                color=color or cesiumpy.color.YELLOW,
            )
        )

        if label:
            self.add_label(position, label, size, color)

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

        self._viewer.entities.add(
            cesiumpy.Polyline(
                positions=cesiumpy.entities.cartesian.Cartesian3Array(
                    functools.reduce(
                        operator.iconcat,
                        [
                            [
                                float(lla.get_longitude().in_degrees()),
                                float(lla.get_latitude().in_degrees()),
                                10.0,
                            ]
                            for lla in map(lla_from_position, positions)
                        ],
                        [],
                    )
                ),
                width=size or 1,
                material=color or cesiumpy.color.YELLOW,
            )
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

        self._viewer.entities.add(
            cesiumpy.Point(
                position=_generate_sampled_position_from_llas(
                    instants=instants,
                    llas=llas,
                ),
                availability=self._get_availability(),
                color=color,
                pixel_size=size,
            )
        )

        return self

    def add_label(
        self,
        position: Position,
        text: str,
        size: int | None = None,
        color: str | None = None,
    ) -> Viewer:
        """
        Add label to Viewer.

        Args:
            position (Position): Label position.
            text (str): Label text.
            size (int, optional): Label size. Defaults to None.
            color (str, optional): Label color. Defaults to None.

        Returns:
            Viewer: The Viewer.
        """

        self._viewer.entities.add(
            cesiumpy.Label(
                position=_cesium_from_ostk_position(position),
                text=text,
                scale=size or 10,
                fill_color=color or cesiumpy.color.WHITE,
            )
        )

        return self

    def render(self) -> str:
        """
        Render Viewer as HTML string.

        Returns:
            str: Rendered HTML string.
        """

        return self._viewer.to_html()

    def _get_availability(self) -> cesiumpy.TimeIntervalCollection:
        """
        Get the availability of the viewer.

        Returns:
            cesiumpy.TimeIntervalCollection: The availability of the viewer.
        """
        return _cesium_from_ostk_intervals(intervals=[self._interval])

    def _repr_html_(self) -> str:
        return self.render()


def _generate_llas(states: list[State]) -> list[LLA]:
    return list(map(lla_from_state, states))


def _generate_sampled_position_from_llas(
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


def _generate_sampled_position_from_states(
    states: list[State],
) -> cesiumpy.SampledPositionProperty:
    """
    Generate a sampled position property from a list of OSTk States.

    Args:
        states (list[State]): A list of OSTk States.

    Returns:
        cesiumpy.SampledPositionProperty: Sampled position property.
    """
    return _generate_sampled_position_from_positions(
        instants=[state.get_instant() for state in states],
        positions=[state.get_position() for state in states],
    )


def _generate_sampled_position_from_positions(
    instants: list[Instant],
    positions: list[Position],
) -> cesiumpy.SampledPositionProperty:
    """
    Generate a sampled position property from a list of OSTk positions and instants.

    Args:
        instants (list[Instant]): A list of OSTk instants.
        positions (list[Position]): A list of OSTk positions.

    Returns:
        cesiumpy.SampledPositionProperty: Sampled position property.
    """
    frame_itrf: Frame = Frame.ITRF()

    return cesiumpy.SampledPositionProperty(
        samples=[
            (
                coerce_to_datetime(instant),
                _cesium_from_ostk_position(
                    position.in_frame(instant=instant, frame=frame_itrf)
                ),
                None,
            )
            for instant, position in zip(instants, positions)
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


def _cesium_from_ostk_sensor(sensor: Sensor) -> cesiumpy.Sensor:
    if isinstance(sensor, ConicSensor):
        return cesiumpy.ConicSensor(
            name=sensor.name,
            direction=cesiumpy.Cartesian3(*sensor.direction),
            half_angle=float(sensor.half_angle.in_radians()),
            length=float(sensor.length.in_meters()),
            material=sensor.color or cesiumpy.color.RED,
        )

    elif isinstance(sensor, RectangularSensor):
        return cesiumpy.RectangularSensor(
            name=sensor.name,
            direction=cesiumpy.Cartesian3(*sensor.direction),
            x_half_angle=float(sensor.x_half_angle.in_radians()),
            y_half_angle=float(sensor.y_half_angle.in_radians()),
            radius=float(sensor.radius.in_meters()),
            material=sensor.color or cesiumpy.color.ORANGE,
        )

    raise NotImplementedError("{sensor.__name__} is not supported yet.")


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


def _cesium_from_ostk_intervals(
    intervals: list[Interval],
) -> cesiumpy.TimeIntervalCollection:
    """
    Convert a list of OSTk intervals into Cesium TimeIntervalCollection.

    Args:
        intervals (list[Interval]): List of OSTk intervals.

    Returns:
        cesiumpy.TimeIntervalCollection: Converted intervals.
    """

    return cesiumpy.TimeIntervalCollection(
        intervals=[
            cesiumpy.TimeInterval(
                start=coerce_to_datetime(interval.get_start()),
                stop=coerce_to_datetime(interval.get_end()),
            )
            for interval in intervals
        ],
    )
