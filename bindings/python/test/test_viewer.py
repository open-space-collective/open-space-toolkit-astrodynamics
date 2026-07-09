# Apache License 2.0

import json

import numpy as np
import pytest

from ostk.physics import Environment
from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import DateTime
from ostk.physics.time import Time
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics import czml
from ostk.astrodynamics.viewer import Viewer
from ostk.astrodynamics.viewer import ConicSensor
from ostk.astrodynamics.viewer import RectangularSensor
from ostk.astrodynamics.viewer import _compute_celestial_angular_diameter_from_states
from ostk.astrodynamics.viewer import _intersection_with_earth
from ostk.astrodynamics.viewer import _quaternion_aligning_z
from ostk.astrodynamics.viewer import _sensor_boundary_directions
from ostk.astrodynamics.viewer import _format_duration


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def orbit(environment: Environment) -> Orbit:
    return Orbit.sun_synchronous(
        epoch=Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC),
        altitude=Length.kilometers(500.0),
        local_time_at_descending_node=Time(14, 0, 0),
        celestial_object=environment.access_celestial_object_with_name("Earth"),
    )


@pytest.fixture
def orbits(environment: Environment) -> list[Orbit]:
    return [
        Orbit.sun_synchronous(
            epoch=Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC),
            altitude=Length.kilometers(500.0),
            local_time_at_descending_node=Time(14, 0, 0),
            celestial_object=environment.access_celestial_object_with_name("Earth"),
        ),
        Orbit.sun_synchronous(
            epoch=Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC),
            altitude=Length.kilometers(500.0),
            local_time_at_descending_node=Time(12, 0, 0),
            celestial_object=environment.access_celestial_object_with_name("Earth"),
        ),
    ]


@pytest.fixture
def profile(orbit: Orbit) -> Profile:
    return Profile.local_orbital_frame_pointing(
        orbit=orbit,
        orbital_frame_type=Orbit.FrameType.VVLH,
    )


@pytest.fixture
def interval() -> Interval:
    return Interval.closed(
        start_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC),
        end_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 10, 0), Scale.UTC),
    )


@pytest.fixture
def viewer(interval: Interval) -> Viewer:
    return Viewer(
        interval=interval,
        zoom_to_entity=False,
        track_entity=False,
    )


def packet_with_id(viewer: Viewer, packet_id: str) -> dict:
    for packet in viewer.to_czml():
        if packet.get("id") == packet_id:
            return packet

    raise AssertionError(f"No packet with id [{packet_id}].")


class TestViewer:
    def test_constructor_success(self, interval: Interval):
        viewer = Viewer(
            interval=interval,
        )

        assert viewer is not None
        assert viewer.interval == interval

        packets: list[dict] = viewer.to_czml()

        assert packets[0]["id"] == "document"
        assert packets[0]["version"] == "1.0"
        assert packets[0]["clock"]["interval"] == (
            "2020-01-01T00:00:00.000000Z/2020-01-01T00:10:00.000000Z"
        )
        assert packets[0]["clock"]["range"] == "LOOP_STOP"

        rendered_html: str = viewer.render()

        assert rendered_html.startswith('<meta charset="utf-8">')
        assert "new Cesium.Viewer" in rendered_html
        assert "Cesium.CzmlDataSource.load" in rendered_html
        assert "sceneModePicker: true" in rendered_html
        assert "sceneMode: Cesium.SceneMode.SCENE3D" in rendered_html
        # Without an Ion token, imagery comes from the CesiumJS distribution itself.
        assert "NaturalEarthII" in rendered_html
        assert rendered_html.endswith("</script>")

    def test_constructor_scene_mode_success(self, interval: Interval):
        rendered_html: str = Viewer(interval=interval, scene_mode="2D").render()

        assert "sceneMode: Cesium.SceneMode.SCENE2D" in rendered_html

        with pytest.raises(ValueError):
            Viewer(interval=interval, scene_mode="4D")

    def test_add_orbit_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
    ):
        viewer.add_orbit(
            orbit=orbit,
            step=Duration.seconds(5.0),
            show_orbital_track=True,
        )

        satellite: dict = packet_with_id(viewer, "satellite")

        assert satellite["billboard"]["image"].startswith("data:image/png;base64,")
        assert satellite["label"]["text"] == "Satellite"
        assert satellite["position"]["interpolationAlgorithm"] == "LAGRANGE"
        assert satellite["position"]["referenceFrame"] == "FIXED"
        assert len(satellite["position"]["cartesian"]) == (600 // 5 + 1) * 4

        # Leading/trailing orbital track: default lead/trail time is one orbital period.
        assert satellite["path"]["leadTime"] == pytest.approx(5677.0, abs=60.0)
        assert satellite["path"]["trailTime"] == pytest.approx(5677.0, abs=60.0)

        rendered_html: str = viewer.render()

        assert rendered_html.startswith('<meta charset="utf-8">')
        assert rendered_html.endswith("</script>")

    def test_add_orbit_lead_trail_time_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
    ):
        viewer.add_orbit(
            orbit=orbit,
            step=Duration.seconds(30.0),
            show_orbital_track=True,
            trail_time=Duration.minutes(10.0),
            lead_time=Duration.minutes(20.0),
        )

        satellite: dict = packet_with_id(viewer, "satellite")

        assert satellite["path"]["trailTime"] == 600.0
        assert satellite["path"]["leadTime"] == 1200.0

    def test_add_orbit_multiple_success(
        self,
        viewer: Viewer,
        orbits: list[Orbit],
    ):
        for i, orbit in enumerate(orbits):
            viewer.add_orbit(
                orbit=orbit,
                step=Duration.seconds(5.0),
                show_orbital_track=True,
                name=f"Satellite {i}",
            )

        assert packet_with_id(viewer, "satellite_0")["label"]["text"] == "Satellite 0"
        assert packet_with_id(viewer, "satellite_1")["label"]["text"] == "Satellite 1"

    def test_add_profile_success(
        self,
        viewer: Viewer,
        profile: Profile,
    ):
        viewer.add_profile(
            profile=profile,
            step=Duration.seconds(30.0),
            show_orbital_track=True,
            cesium_asset_id=123,
            sensors=[
                ConicSensor(
                    name="star_tracker",
                    direction=(1.0, 0.0, 0.0),
                    half_angle=Angle.degrees(8.0),
                    length=Length.meters(0.1),
                    color="red",
                ),
            ],
            show_xyz_axes=True,
        )

        satellite: dict = packet_with_id(viewer, "satellite")

        assert satellite["properties"]["ion_asset_id"] == 123
        assert "unitQuaternion" in satellite["orientation"]
        assert len(satellite["orientation"]["unitQuaternion"]) == (600 // 30 + 1) * 5
        assert "path" in satellite

        # Sensors are rendered as cone (cylinder) entities attached to the satellite.
        star_tracker: dict = packet_with_id(viewer, "satellite_star_tracker")

        assert star_tracker["cylinder"]["topRadius"] == 0.0
        assert star_tracker["cylinder"]["length"] == pytest.approx(0.1)

        for axis in ("x_axis", "y_axis", "z_axis"):
            assert packet_with_id(viewer, f"satellite_{axis}")["cylinder"] is not None

        rendered_html: str = viewer.render()

        assert 'entity.properties.hasProperty("ion_asset_id")' in rendered_html
        assert "Cesium.IonResource.fromAssetId" in rendered_html

    def test_add_profile_without_asset_id_warns(
        self,
        viewer: Viewer,
        profile: Profile,
    ):
        with pytest.warns(FutureWarning):
            viewer.add_profile(
                profile=profile,
                step=Duration.seconds(60.0),
            )

        satellite: dict = packet_with_id(viewer, "satellite")

        assert satellite["billboard"]["image"].startswith("data:image/png;base64,")

    def test_add_profile_rectangular_sensor_success(
        self,
        viewer: Viewer,
        profile: Profile,
    ):
        viewer.add_profile(
            profile=profile,
            step=Duration.seconds(60.0),
            cesium_asset_id=123,
            sensors=[
                RectangularSensor(
                    name="camera",
                    direction=(0.0, 0.0, 1.0),
                    x_half_angle=Angle.degrees(10.0),
                    y_half_angle=Angle.degrees(5.0),
                    radius=Length.kilometers(600.0),
                    color="orange",
                ),
            ],
        )

        for corner_index in range(4):
            edge: dict = packet_with_id(viewer, f"satellite_camera_edge_{corner_index}")

            assert edge["polyline"]["positions"]["references"] == [
                "satellite#position",
                f"satellite_camera_corner_{corner_index}#position",
            ]

    def test_add_sensor_projection_success(
        self,
        viewer: Viewer,
        profile: Profile,
    ):
        viewer.add_sensor_projection(
            profile=profile,
            sensor=ConicSensor(
                name="payload",
                direction=(0.0, 0.0, 1.0),  # +Z is nadir for a VVLH-pointing profile
                half_angle=Angle.degrees(30.0),
                length=Length.meters(1.0),
                color="red",
            ),
            time_step=Duration.seconds(60.0),
        )

        projection: dict = packet_with_id(viewer, "payload_projection")

        assert len(projection["polygon"]["positions"]) == 10
        assert projection["polygon"]["positions"][0]["cartesian"]

        # The nadir-pointing footprint boundary must lie on the Earth ellipsoid.
        first_footprint: list[float] = projection["polygon"]["positions"][0]["cartesian"]
        point: np.ndarray = np.array(first_footprint[0:3])

        assert np.linalg.norm(point) == pytest.approx(6.378137e6, rel=1e-2)

        show_entries = projection["polygon"]["show"]

        assert show_entries[0]["boolean"] is False  # default over the whole interval
        assert all(entry["boolean"] for entry in show_entries[1:])

    @pytest.mark.parametrize(
        "celestial_body_name",
        ["Earth", "Moon", "Sun"],
    )
    def test_add_celestial_body_direction_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
        celestial_body_name: str,
        environment: Environment,
    ):
        viewer.add_celestial_body_direction(
            profile_or_trajectory=orbit,
            time_step=Duration.seconds(30.0),
            celestial=environment.access_celestial_object_with_name(celestial_body_name),
        )

        direction: dict = packet_with_id(
            viewer, f"{celestial_body_name.lower()}_direction"
        )

        assert direction["polyline"]["positions"]["references"] == [
            f"{celestial_body_name.lower()}_direction_start#position",
            f"{celestial_body_name.lower()}_direction_end#position",
        ]

    def test_add_target_vector_success(
        self,
        viewer: Viewer,
        orbits: list[Orbit],
    ):
        viewer.add_target_vector(
            profile_or_trajectory=orbits[0],
            target=orbits[1],
            name="inter_satellite_link",
            color="cyan",
            time_step=Duration.seconds(60.0),
        )

        vector: dict = packet_with_id(viewer, "inter_satellite_link")

        assert vector["polyline"]["arcType"] == "NONE"
        assert vector["polyline"]["material"]["solidColor"]["color"]["rgba"] == [
            0,
            255,
            255,
            255,
        ]

        start: dict = packet_with_id(viewer, "inter_satellite_link_start")

        assert len(start["position"]["cartesian"]) == (600 // 60 + 1) * 4

    def test_add_accesses_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
        interval: Interval,
    ):
        access_intervals: list[Interval] = [
            Interval.closed(
                start_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 1, 0), Scale.UTC),
                end_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 3, 0), Scale.UTC),
            ),
            Interval.closed(
                start_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 6, 0), Scale.UTC),
                end_instant=Instant.date_time(DateTime(2020, 1, 1, 0, 8, 0), Scale.UTC),
            ),
        ]

        viewer.add_accesses(
            accesses=access_intervals,
            observer=orbit,
            target=Position.meters([6378137.0, 0.0, 0.0], Frame.ITRF()),
            name="Ground Station Access",
            time_step=Duration.seconds(30.0),
        )

        access: dict = packet_with_id(viewer, "ground_station_access")

        assert access["polyline"]["positions"]["references"] == [
            "ground_station_access_observer#position",
            "ground_station_access_target#position",
        ]

        show_entries: list[dict] = access["polyline"]["show"]

        assert show_entries[0]["boolean"] is False
        assert show_entries[1]["interval"] == (
            "2020-01-01T00:01:00.000000Z/2020-01-01T00:03:00.000000Z"
        )
        assert show_entries[1]["boolean"] is True
        assert len(show_entries) == 3

        assert "<table" in access["description"]
        assert "00:02:00" in access["description"]

        label: dict = packet_with_id(viewer, "ground_station_access_label")

        assert label["position"]["reference"] == "ground_station_access_target#position"
        assert len(label["label"]["text"]) == 2
        assert "Ground Station Access #1" in label["label"]["text"][0]["string"]

    def test_add_ground_tracks_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
    ):
        viewer.add_ground_tracks(profile_or_trajectory=orbit)

        line: dict = packet_with_id(viewer, "line")

        assert line["polyline"]["positions"]["cartographicDegrees"]

        moving_point: dict = packet_with_id(viewer, "moving_point")

        assert moving_point["point"]["color"]["rgba"] == [255, 140, 0, 255]

    def test_add_target_success(
        self,
        viewer: Viewer,
    ):
        viewer.add_target(
            position=Position.meters([1.0, 2.0, 3.0], Frame.ITRF()),
            size=123,
            color="red",
        )

        target: dict = packet_with_id(viewer, "target")

        assert target["point"]["pixelSize"] == 123
        assert target["point"]["color"]["rgba"] == [255, 0, 0, 255]
        assert target["position"]["cartographicDegrees"][0] == pytest.approx(
            63.43494882292201
        )
        assert target["position"]["cartographicDegrees"][1] == pytest.approx(
            18.22447811510915
        )

    def test_add_target_with_label_success(
        self,
        viewer: Viewer,
    ):
        viewer.add_target(
            position=Position.meters([1.0, 2.0, 3.0], Frame.ITRF()),
            size=123,
            color="red",
            label="TEST",
        )

        label: dict = packet_with_id(viewer, "test_label")

        assert label["label"]["text"] == "TEST"
        assert label["label"]["fillColor"]["rgba"] == [255, 0, 0, 255]

    def test_add_line_success(
        self,
        viewer: Viewer,
    ):
        viewer.add_line(
            positions=[
                Position.meters([1.0, 2.0, 3.0], Frame.ITRF()),
                Position.meters([4.0, 5.0, 6.0], Frame.ITRF()),
            ],
            size=10,
            color="red",
        )

        line: dict = packet_with_id(viewer, "line")

        assert line["polyline"]["width"] == 10
        assert line["polyline"]["positions"]["cartographicDegrees"] == pytest.approx(
            [
                63.43494882292201,
                18.22447811510915,
                10.0,
                51.34019174590991,
                10.165199393640696,
                10.0,
            ]
        )

    def test_add_label_success(
        self,
        viewer: Viewer,
    ):
        viewer.add_label(
            position=Position.meters([6671000.0, 0.0, 0.0], Frame.ITRF()),
            text="Hello, World!",
            size=1,
            color="red",
        )

        label: dict = packet_with_id(viewer, "hello_world_label")

        assert label["label"]["text"] == "Hello, World!"
        assert label["label"]["scale"] == 1

    def test_to_czml_is_json_serializable(
        self,
        viewer: Viewer,
        orbit: Orbit,
    ):
        viewer.add_orbit(
            orbit=orbit,
            step=Duration.seconds(30.0),
            show_orbital_track=True,
        )

        assert json.loads(json.dumps(viewer.to_czml())) == viewer.to_czml()

    def test_save_czml_success(
        self,
        viewer: Viewer,
        orbit: Orbit,
        tmp_path,
    ):
        viewer.add_orbit(
            orbit=orbit,
            step=Duration.seconds(30.0),
        )

        path: str = str(tmp_path / "scenario.czml")
        viewer.save_czml(path)

        with open(path) as file:
            packets: list[dict] = json.load(file)

        assert packets[0]["id"] == "document"


class TestCzml:
    def test_format_instant_success(self):
        assert (
            czml.format_instant(
                Instant.date_time(DateTime(2020, 1, 1, 0, 0, 0), Scale.UTC)
            )
            == "2020-01-01T00:00:00.000000Z"
        )

    def test_format_color_success(self):
        assert czml.format_color("red") == {"rgba": [255, 0, 0, 255]}
        assert czml.format_color("#00FF00") == {"rgba": [0, 255, 0, 255]}
        assert czml.format_color("#00FF0080") == {"rgba": [0, 255, 0, 128]}
        assert czml.format_color((1, 2, 3)) == {"rgba": [1, 2, 3, 255]}
        assert czml.format_color([1, 2, 3, 4]) == {"rgba": [1, 2, 3, 4]}
        assert czml.format_color(None, default="yellow") == {"rgba": [255, 255, 0, 255]}
        assert czml.format_color("red", alpha=0.5) == {"rgba": [255, 0, 0, 128]}

        with pytest.raises(ValueError):
            czml.format_color("not-a-color")

    def test_interval_booleans_success(self, interval: Interval):
        assert czml.interval_booleans(interval, []) is False

        entries = czml.interval_booleans(
            interval,
            [
                Interval.closed(
                    start_instant=Instant.date_time(
                        DateTime(2020, 1, 1, 0, 1, 0), Scale.UTC
                    ),
                    end_instant=Instant.date_time(
                        DateTime(2020, 1, 1, 0, 2, 0), Scale.UTC
                    ),
                )
            ],
        )

        assert entries[0]["boolean"] is False
        assert entries[1]["boolean"] is True


class TestHelpers:
    def test_intersection_with_earth_success(self):
        # Nadir ray from 500 km above the equator hits the ellipsoid surface.
        intersection: np.ndarray = _intersection_with_earth(
            origin=np.array([6878137.0, 0.0, 0.0]),
            direction=np.array([-1.0, 0.0, 0.0]),
        )

        assert intersection == pytest.approx([6378137.0, 0.0, 0.0])

        # A ray pointing away from the Earth misses.
        assert (
            _intersection_with_earth(
                origin=np.array([6878137.0, 0.0, 0.0]),
                direction=np.array([1.0, 0.0, 0.0]),
            )
            is None
        )

    def test_quaternion_aligning_z_success(self):
        for direction in (
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, -1.0],
            [1.0, 2.0, 3.0],
        ):
            x, y, z, w = _quaternion_aligning_z(np.array(direction))

            # Rotate +Z by the quaternion (Hamilton convention) and compare.
            v = np.array([0.0, 0.0, 1.0])
            q_vector = np.array([x, y, z])
            rotated = (
                v * (w**2 - q_vector @ q_vector)
                + 2.0 * q_vector * (q_vector @ v)
                + 2.0 * w * np.cross(q_vector, v)
            )

            expected = np.array(direction) / np.linalg.norm(direction)

            assert rotated == pytest.approx(expected)

    def test_sensor_boundary_directions_success(self):
        conic_directions = _sensor_boundary_directions(
            ConicSensor(
                name="payload",
                direction=(0.0, 0.0, 1.0),
                half_angle=Angle.degrees(30.0),
                length=Length.meters(1.0),
                color="red",
            )
        )

        assert len(conic_directions) == 32
        for direction in conic_directions:
            assert np.linalg.norm(direction) == pytest.approx(1.0)
            assert np.arccos(direction @ np.array([0.0, 0.0, 1.0])) == pytest.approx(
                np.deg2rad(30.0)
            )

        rectangular_directions = _sensor_boundary_directions(
            RectangularSensor(
                name="camera",
                direction=(0.0, 0.0, 1.0),
                x_half_angle=Angle.degrees(10.0),
                y_half_angle=Angle.degrees(5.0),
                radius=Length.kilometers(600.0),
                color="orange",
            )
        )

        assert len(rectangular_directions) == 16

    def test_format_duration_success(self):
        assert _format_duration(Duration.seconds(3725.0)) == "01:02:05"


def test_compute_celestial_angular_diameter_from_states_success(
    orbit: Orbit,
    interval: Interval,
    environment: Environment,
) -> None:
    assert _compute_celestial_angular_diameter_from_states(
        celestial=environment.access_celestial_object_with_name("Sun"),
        states=orbit.get_states_at(
            interval.generate_grid(Duration.seconds(30.0)),
        ),
    ).mean() == pytest.approx(0.54, rel=1e-2)
