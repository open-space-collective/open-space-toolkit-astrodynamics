# Apache License 2.0

import pytest

from ostk.physics import Environment
from ostk.physics.units import Length
from ostk.physics.units import Angle
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.time import DateTime
from ostk.physics.time import Time
from ostk.physics.time import Scale
from ostk.physics.units import Length
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.viewer import Viewer
from ostk.astrodynamics.viewer import ConicSensor


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
def profile(orbit: Orbit) -> Profile:
    return Profile.nadir_pointing(
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
    )


class TestViewer:
    def test_constructor_success(self, interval: Interval):
        viewer = Viewer(
            interval=interval,
        )

        assert viewer is not None
        assert viewer.interval == interval

        rendered_html: str = viewer.render()

        assert rendered_html.startswith('<meta charset="utf-8">')
        assert "var widget = new Cesium.Viewer" in rendered_html
        assert rendered_html.endswith("</script>")

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
        )

        rendered_html: str = viewer.render()

        assert rendered_html.startswith('<meta charset="utf-8">')
        assert "var widget = new Cesium.Viewer" in rendered_html
        assert " widget.entities.add({position: widget" in rendered_html
        assert "Cesium.IonResource.fromAssetId(123)" in rendered_html
        assert "widget.entities.add({polyline:" in rendered_html
        assert (
            "widget.entities.add({position: widget.star_tracker_position" in rendered_html
        )
        assert rendered_html.endswith("</script>")

    def test_add_target_success(
        self,
        viewer: Viewer,
    ):
        viewer.add_target(
            position=Position.meters([1.0, 2.0, 3.0], Frame.ITRF()),
            size=123,
            color="red",
        )

        rendered_html: str = viewer.render()

        assert rendered_html.startswith('<meta charset="utf-8">')
        assert "var widget = new Cesium.Viewer" in rendered_html
        assert (
            "widget.entities.add({position: Cesium.Cartesian3.fromDegrees(63.43494882292201, 18.22447811510915, -6376045.535225509), point: {pixelSize: 123.0, color: Cesium.Color.RED}});"
            in rendered_html
        )
        assert rendered_html.endswith("</script>")
