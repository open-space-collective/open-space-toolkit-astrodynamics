# Apache License 2.0

import pytest

from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import Instant
from ostk.physics.time import Time
from ostk.physics.environment.gravitational import Earth
from ostk.physics.environment.object.celestial import Sun
from ostk.physics.environment.object import Celestial
from ostk.physics import Environment

from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def semi_major_axis() -> Length:
    return Length.kilometers(7000.0)


@pytest.fixture
def eccentricity() -> float:
    return 0.1


@pytest.fixture
def inclination() -> Angle:
    return Angle.degrees(35.0)


@pytest.fixture
def raan() -> Angle:
    return Angle.degrees(40.0)


@pytest.fixture
def aop() -> Angle:
    return Angle.degrees(50.0)


@pytest.fixture
def true_anomaly() -> Angle:
    return Angle.degrees(60.0)


@pytest.fixture
def coe(
    semi_major_axis: Length,
    eccentricity: float,
    inclination: Angle,
    raan: Angle,
    aop: Angle,
    true_anomaly: Angle,
) -> COE:
    return COE(semi_major_axis, eccentricity, inclination, raan, aop, true_anomaly)


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


class TestCOE:
    def test_constructors(
        self,
        semi_major_axis: Length,
        eccentricity: float,
        inclination: Angle,
        raan: Angle,
        aop: Angle,
        true_anomaly: Angle,
    ):
        coe: COE = COE(
            semi_major_axis,
            eccentricity,
            inclination,
            raan,
            aop,
            true_anomaly,
        )

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined()

    def test_comparators(
        self,
        coe: COE,
    ):
        assert coe == coe
        assert (coe != coe) is False

    def test_getters(
        self,
        coe: COE,
        semi_major_axis: Length,
        eccentricity: float,
        inclination: Angle,
        raan: Angle,
        aop: Angle,
        true_anomaly: Angle,
    ):
        assert coe.get_semi_major_axis() == semi_major_axis
        assert coe.get_eccentricity() == eccentricity
        assert coe.get_inclination() == inclination
        assert coe.get_raan() == raan
        assert coe.get_aop() == aop
        assert coe.get_true_anomaly() == true_anomaly
        assert coe.get_argument_of_latitude() == aop + true_anomaly
        assert coe.get_mean_anomaly() is not None
        assert coe.get_eccentric_anomaly() is not None
        assert coe.get_mean_motion(Earth.EGM2008.gravitational_parameter) is not None
        assert coe.get_orbital_period(Earth.EGM2008.gravitational_parameter) is not None
        assert coe.get_periapsis_radius() is not None
        assert coe.get_apoapsis_radius() is not None
        assert coe.get_semi_latus_rectum() is not None
        assert coe.get_radial_distance() is not None
        assert coe.get_angular_momentum(Earth.EGM2008.gravitational_parameter) is not None
        assert coe.get_SI_vector(COE.AnomalyType.TrueAnomaly) is not None
        assert coe.get_SI_vector(COE.AnomalyType.MeanAnomaly) is not None
        assert coe.get_SI_vector(COE.AnomalyType.EccentricAnomaly) is not None

    def test_anomaly_conversions(self):
        assert (
            COE.eccentric_anomaly_from_true_anomaly(Angle.degrees(0.0), 0.0) is not None
        )
        assert (
            COE.true_anomaly_from_eccentric_anomaly(Angle.degrees(0.0), 0.0) is not None
        )
        assert (
            COE.mean_anomaly_from_eccentric_anomaly(Angle.degrees(0.0), 0.0) is not None
        )
        assert (
            COE.eccentric_anomaly_from_mean_anomaly(Angle.degrees(0.0), 0.0, 0.0)
            is not None
        )

    def test_compute_methods(self, earth: Celestial):
        semi_latus_rectum: float = COE.compute_semi_latus_rectum(7000.0e3, 0.0)
        assert semi_latus_rectum is not None
        assert (
            COE.compute_angular_momentum(
                7000.0e3, 0.0, Earth.EGM2008.gravitational_parameter
            )
            is not None
        )
        assert (
            COE.compute_angular_momentum(
                semi_latus_rectum, Earth.EGM2008.gravitational_parameter
            )
            is not None
        )
        assert COE.compute_radial_distance(7000.0e3, 0.0, 0.0) is not None

        assert COE.compute_ltan(Angle.degrees(270.0), Instant.J2000()) is not None
        assert (
            COE.compute_ltan(Angle.degrees(270.0), Instant.J2000(), Sun.default())
            is not None
        )
        assert COE.compute_mean_ltan(Angle.degrees(270.0), Instant.J2000()) is not None
        assert (
            COE.compute_mean_ltan(Angle.degrees(270.0), Instant.J2000(), Sun.default())
            is not None
        )
        assert COE.compute_ltdn(Angle.degrees(270.0), Instant.J2000()) is not None
        assert (
            COE.compute_ltdn(Angle.degrees(270.0), Instant.J2000(), Sun.default())
            is not None
        )
        assert COE.compute_mean_ltdn(Angle.degrees(270.0), Instant.J2000()) is not None
        assert (
            COE.compute_mean_ltdn(Angle.degrees(270.0), Instant.J2000(), Sun.default())
            is not None
        )
        assert (
            COE.compute_sun_synchronous_inclination(Length.meters(7130982.0), 0.0, earth)
            is not None
        )
        assert (
            COE.compute_raan_from_ltan(Time.parse("12:00:00"), Instant.J2000(), earth)
            is not None
        )

    def test_from_SI_vector(
        self,
        coe: COE,
        semi_major_axis: Length,
        eccentricity: float,
        inclination: Angle,
        raan: Angle,
        aop: Angle,
        true_anomaly: Angle,
    ):
        assert coe == COE.from_SI_vector(
            [
                semi_major_axis.in_meters(),
                eccentricity,
                inclination.in_radians(),
                raan.in_radians(),
                aop.in_radians(),
                true_anomaly.in_radians(),
            ],
            COE.AnomalyType.TrueAnomaly,
        )

    def test_string_from_element(self):
        element_str = COE.string_from_element(COE.Element.SemiMajorAxis)
        assert element_str == "SemiMajorAxis"

    def test_sun_synchronous(self, earth: Celestial):
        semi_major_axis: Length = Length.meters(7130982.0)
        local_time_at_ascending_node: Time = Time.parse("12:00:00")
        epoch: Instant = Instant.J2000()
        eccentricity: float = 0.0
        argument_of_latitude: Angle = Angle.degrees(30.0)

        assert (
            COE.sun_synchronous(
                semi_major_axis,
                local_time_at_ascending_node,
                epoch,
                earth,
                eccentricity,
                argument_of_latitude,
            )
            is not None
        )
        assert (
            COE.sun_synchronous(
                semi_major_axis, local_time_at_ascending_node, epoch, earth, eccentricity
            )
            is not None
        )
        assert (
            COE.sun_synchronous(
                semi_major_axis, local_time_at_ascending_node, epoch, earth
            )
            is not None
        )

    def test_geo_synchronous(self, earth: Celestial):
        epoch: Instant = Instant.J2000()
        inclination: Angle = Angle.degrees(0.01)
        longitude: Angle = Angle.degrees(0.0)

        assert COE.geo_synchronous(epoch, inclination, longitude, earth) is not None

    def test_circular(self):
        semi_major_axis: Length = Length.kilometers(7000.0)
        inclination: Angle = Angle.degrees(45.0)
        argument_of_latitude: Angle = Angle.degrees(30.0)

        assert (
            COE.circular(semi_major_axis, inclination, argument_of_latitude) is not None
        )
        assert COE.circular(semi_major_axis, inclination) is not None
        assert COE.circular(semi_major_axis) is not None

    def test_equatorial(self):
        semi_major_axis: Length = Length.kilometers(7000.0)
        eccentricity: float = 0.1
        true_anomaly: Angle = Angle.degrees(45.0)

        assert COE.equatorial(semi_major_axis, eccentricity, true_anomaly) is not None
        assert COE.equatorial(semi_major_axis, eccentricity) is not None
        assert COE.equatorial(semi_major_axis) is not None

    def test_undefined(self):
        coe: COE = COE.undefined()

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined() is False

    def test_frozen_orbit(
        self,
        semi_major_axis: Length,
        inclination: Angle,
    ):
        coe: COE = COE.frozen_orbit(
            semi_major_axis,
            Earth.EGM2008.equatorial_radius,
            Earth.EGM2008.J2,
            Earth.EGM2008.J3,
            inclination=inclination,
        )

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined()

        coe: COE = COE.frozen_orbit(
            semi_major_axis,
            Environment.default().access_celestial_object_with_name("Earth"),
        )

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined()
