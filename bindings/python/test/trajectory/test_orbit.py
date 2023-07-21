# Apache License 2.0

import pytest

from ostk.physics import Environment
from ostk.physics.units import Length, Angle
from ostk.physics.time import Scale, Instant, DateTime, Time

from ostk.astrodynamics.trajectory import Orbit, State
from ostk.astrodynamics.trajectory.orbit.models import SGP4
from ostk.astrodynamics.trajectory.orbit.models.sgp4 import TLE


@pytest.fixture
def earth():
    return Environment.default().access_celestial_object_with_name("Earth")


class TestOrbit:
    def test_trajectory_orbit_constructors(self, earth):
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

    def test_trajectory_orbit_circular(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)
        inclination = Angle.degrees(45.0)

        orbit: Orbit = Orbit.circular(epoch, altitude, inclination, earth)

    def test_trajectory_orbit_equatorial(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        apoapsis_altitude = Length.kilometers(500.1)
        periapsis_altitude = Length.kilometers(499.9)

        orbit: Orbit = Orbit.equatorial(
            epoch, apoapsis_altitude, periapsis_altitude, earth
        )

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_trajectory_orbit_circular_equatorial(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)

        orbit: Orbit = Orbit.circular_equatorial(epoch, altitude, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_trajectory_orbit_geo_synchronous(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        inclination = Angle.degrees(45.0)
        longitude = Angle.degrees(45.0)

        orbit: Orbit = Orbit.geo_synchronous(epoch, inclination, longitude, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

    def test_trajectory_orbit_sun_synchronous(self, earth):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        altitude = Length.kilometers(500.0)
        local_time_at_descending_node = Time.midnight()

        orbit: Orbit = Orbit.sun_synchronous(
            epoch, altitude, local_time_at_descending_node, earth
        )

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()
