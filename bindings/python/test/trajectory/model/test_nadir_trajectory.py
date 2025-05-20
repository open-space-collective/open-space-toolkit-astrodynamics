# Apache License 2.0

import pytest

from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.unit import Length
from ostk.physics.unit import Angle

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.model import Nadir
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def orbit() -> Orbit:
    earth = Earth.WGS84()
    coe = COE(
        semi_major_axis=Length.kilometers(7000.0),
        eccentricity=0.0,
        inclination=Angle.degrees(98.0),
        raan=Angle.degrees(0.0),
        aop=Angle.degrees(0.0),
        true_anomaly=Angle.degrees(0.0),
    )
    keplerian_model = Kepler(
        coe=coe,
        epoch=Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
        gravitational_parameter=earth.get_gravitational_parameter(),
        equatorial_radius=earth.get_equatorial_radius(),
        j2=earth.get_j2(),
        j4=earth.get_j4(),
        perturbation_type=Kepler.PerturbationType.No,
    )
    return Orbit(keplerian_model, earth)


@pytest.fixture
def nadir(orbit: Orbit) -> Nadir:
    return Nadir(orbit)


class TestNadir:
    def test_constructor(
        self,
        nadir: Nadir,
    ):
        assert nadir.is_defined()

    def test_calculate_state_at(
        self,
        nadir: Nadir,
    ):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

        state: State = nadir.calculate_state_at(instant)

        assert state is not None

    def test_equality_operator(
        self,
        nadir: Nadir,
    ):
        assert nadir == nadir

    def test_inequality_operator(
        self,
        nadir: Nadir,
    ):
        nadir2 = Nadir(Orbit.undefined())
        assert nadir != nadir2

    def test_get_orbit(
        self,
        nadir: Nadir,
    ):
        assert nadir.get_orbit() is not None

    def test_get_step_size(
        self,
        nadir: Nadir,
    ):
        assert nadir.get_step_size() is not None
