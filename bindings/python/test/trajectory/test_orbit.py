# Apache License 2.0

import pytest

from ostk.physics.environment.object.celestial import Earth
from ostk.physics.unit import Length, Angle
from ostk.physics.time import Scale, Instant, DateTime, Time, Duration, Interval

from ostk.astrodynamics.trajectory import Orbit, State
from ostk.astrodynamics.trajectory.orbit import Pass
from ostk.astrodynamics.trajectory.orbit import Pass
from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE


@pytest.fixture
def earth() -> Earth:
    return Earth.default()


@pytest.fixture
def epoch() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def orbit(earth: Earth, epoch: Instant):
    return Orbit.sun_synchronous(epoch, Length.kilometers(500.0), Time.midnight(), earth)


@pytest.fixture
def states(orbit: Orbit, epoch: Instant) -> list[State]:
    instants: list[Instant] = Interval.closed(
        epoch, epoch + Duration.days(1.0)
    ).generate_grid(Duration.seconds(20.0))

    return orbit.get_states_at(instants)


class TestOrbit:
    def test_constructors(self, earth):
        # Construct Two-Line Element set
        tle = TLE(
            "1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994",
            "2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316",
        )

        # Construct orbit using SGP4 model
        orbit = Orbit(SGP4(tle), earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

        # Construct get state at current epoch
        state: State = orbit.get_state_at(Instant.now())

        assert state is not None
        assert isinstance(state, State)

    @pytest.mark.parametrize(
        "frame_type",
        [
            (Orbit.FrameType.NED),
            (Orbit.FrameType.LVLH),
            (Orbit.FrameType.VVLH),
            (Orbit.FrameType.LVLHGD),
            (Orbit.FrameType.LVLHGDGT),
            (Orbit.FrameType.QSW),
            (Orbit.FrameType.TNW),
            (Orbit.FrameType.VNC),
        ],
    )
    def test_get_orbital_frame(self, orbit: Orbit, frame_type: Orbit.FrameType):
        assert orbit.get_orbital_frame(frame_type) is not None

    def test_get_revolution_number_at(self, orbit: Orbit):
        assert (
            orbit.get_revolution_number_at(
                Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
            )
            == 1
        )

    def test_get_pass_at(self, orbit: Orbit):
        pass_ = orbit.get_pass_at(
            Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        )

        assert pass_ is not None
        assert isinstance(pass_, Pass)
        assert pass_.is_defined()

    def test_get_pass_with_revolution_number(self, orbit: Orbit):
        pass_ = orbit.get_pass_with_revolution_number(1)

        assert pass_ is not None
        assert isinstance(pass_, Pass)
        assert pass_.is_defined()

    def test_undefined(self):
        assert Orbit.undefined().is_defined() is False

    def test_circular(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)
        inclination = Angle.degrees(45.0)

        orbit: Orbit = Orbit.circular(epoch, altitude, inclination, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_equatorial(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        apoapsis_altitude = Length.kilometers(500.1)
        periapsis_altitude = Length.kilometers(499.9)

        orbit: Orbit = Orbit.equatorial(
            epoch, apoapsis_altitude, periapsis_altitude, earth
        )

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_circular_equatorial(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)

        orbit: Orbit = Orbit.circular_equatorial(epoch, altitude, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_geo_synchronous(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        inclination = Angle.degrees(45.0)
        longitude = Angle.degrees(45.0)

        orbit: Orbit = Orbit.geo_synchronous(epoch, inclination, longitude, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_sun_synchronous(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)
        local_time_at_descending_node = Time.midnight()

        orbit: Orbit = Orbit.sun_synchronous(
            epoch, altitude, local_time_at_descending_node, earth
        )

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

        assert Orbit.sun_synchronous(
            epoch=epoch,
            altitude=altitude,
            local_time_at_descending_node=local_time_at_descending_node,
            celestial_object=earth,
            argument_of_latitude=Angle.degrees(50.0),
        ).is_defined()

    def test_compute_passes(self, orbit: Orbit, states: list[State]):
        passes: list[tuple[int, Pass]] = orbit.compute_passes(states, 1)
        assert passes is not None
