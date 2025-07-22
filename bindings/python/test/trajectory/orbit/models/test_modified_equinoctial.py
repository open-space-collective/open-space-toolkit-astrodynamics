# Apache License 2.0

import pytest

from ostk.physics.unit import Length, Angle, Derived
from ostk.physics.coordinate import Position, Velocity, Frame
from ostk.physics.environment.object.celestial import Earth

from ostk.astrodynamics.trajectory.orbit.model import ModifiedEquinoctial
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def earth_gravitational_parameter() -> Derived:
    return Earth.default().get_gravitational_parameter()


@pytest.fixture
def gcrf() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def meoe_default_values():
    return {
        "p": Length.kilometers(7500.0),
        "f": 0.01,
        "g": 0.02,
        "h": 0.03,
        "k": 0.04,
        "L": Angle.degrees(45.0),
    }


@pytest.fixture
def reference_coe() -> COE:
    sma_ref = Length.kilometers(7000.0)
    ecc_ref = 0.0  # Circular
    inc_ref = Angle.degrees(28.5)
    raan_ref = Angle.degrees(10.0)
    aop_ref = Angle.degrees(0.0)  # Typically undefined for circular, set to 0 for COE
    ta_ref = Angle.degrees(45.0)  # Argument of latitude

    return COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)


@pytest.fixture
def reference_cartesian_state(
    reference_coe: COE,
    earth_gravitational_parameter: Derived,
    gcrf: Frame,
) -> tuple[Position, Velocity]:
    return reference_coe.get_cartesian_state(
        earth_gravitational_parameter,
        gcrf,
    )


@pytest.fixture
def modified_equinoctial(meoe_default_values: dict) -> ModifiedEquinoctial:
    return ModifiedEquinoctial(
        meoe_default_values["p"],
        meoe_default_values["f"],
        meoe_default_values["g"],
        meoe_default_values["h"],
        meoe_default_values["k"],
        meoe_default_values["L"],
    )


class TestModifiedEquinoctial:

    def test_constructor_and_getters(
        self,
        modified_equinoctial: ModifiedEquinoctial,
        meoe_default_values: dict,
    ):
        assert modified_equinoctial.is_defined()
        assert modified_equinoctial.get_semi_latus_rectum() == meoe_default_values["p"]
        assert modified_equinoctial.get_eccentricity_x() == meoe_default_values["f"]
        assert modified_equinoctial.get_eccentricity_y() == meoe_default_values["g"]
        assert modified_equinoctial.get_node_x() == meoe_default_values["h"]
        assert modified_equinoctial.get_node_y() == meoe_default_values["k"]
        assert modified_equinoctial.get_true_longitude() == meoe_default_values["L"]

    def test_undefined_state(self):
        meoe_undefined = ModifiedEquinoctial.undefined()
        assert not meoe_undefined.is_defined()

    def test_get_si_vector(
        self,
        modified_equinoctial: ModifiedEquinoctial,
    ):
        si_vector = modified_equinoctial.get_si_vector()
        assert len(si_vector) == 6

    def test_cartesian(
        self,
        reference_cartesian_state: tuple[Position, Velocity],
        earth_gravitational_parameter: Derived,
    ):
        assert (
            ModifiedEquinoctial.cartesian(
                reference_cartesian_state,
                earth_gravitational_parameter,
            )
            is not None
        )

    def test_get_cartesian_state(
        self,
        modified_equinoctial: ModifiedEquinoctial,
        gcrf: Frame,
        earth_gravitational_parameter: Derived,
    ):
        assert (
            modified_equinoctial.get_cartesian_state(
                earth_gravitational_parameter,
                gcrf,
            )
            is not None
        )

    def test_coe_method_general_elliptical(self):
        sma = Length.kilometers(24396.137)
        ecc = 0.7308
        inc = Angle.degrees(7.0)
        raan = Angle.degrees(0.0)
        aop = Angle.degrees(90.0)
        ta = Angle.degrees(0.0)

        coe = COE(sma, ecc, inc, raan, aop, ta)

        assert ModifiedEquinoctial.coe(coe) is not None

    def test_to_coe(
        self,
        modified_equinoctial: ModifiedEquinoctial,
        earth_gravitational_parameter: Derived,
    ):
        coe = modified_equinoctial.to_coe(earth_gravitational_parameter)
        assert coe.is_defined()
