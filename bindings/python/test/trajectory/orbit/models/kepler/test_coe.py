# Apache License 2.0

import pytest

from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.environment.gravitational import Earth

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


class TestCOE:
    def test_constructors(self):
        a = Length.kilometers(7000.0)
        e = 0.1
        i = Angle.degrees(35.0)
        raan = Angle.degrees(40.0)
        aop = Angle.degrees(50.0)
        nu = Angle.degrees(60.0)

        coe: COE = COE(a, e, i, raan, aop, nu)

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined()

        coe: COE = COE.undefined()

        assert coe is not None
        assert isinstance(coe, COE)
        assert coe.is_defined() is False

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

    def test_compute_methods(self):
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
