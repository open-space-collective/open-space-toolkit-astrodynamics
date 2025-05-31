import pytest
import numpy as np
import math

from ostk.core.type import Real, String

from ostk.physics.unit import Length, Angle, Derived
from ostk.physics.time import Instant
from ostk.physics.coordinate import Position, Velocity, Frame
from ostk.physics.environment.object.celestial import Earth
from ostk.core.container import Pair

from ostk.astrodynamics.trajectory.orbit.model import MEOE
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


# Tolerance for floating point comparisons
POSITION_TOLERANCE_M = 1e-7  # meters
VELOCITY_TOLERANCE_MPS = 1e-10 # m/s
MEOE_ELEMENT_TOLERANCE = 1e-10 # For f, g, h, k elements
ANGLE_TOLERANCE_RAD = 1e-10    # For L element

def normalize_angle_rad(angle_rad: float) -> float:
    """Normalize angle to [-pi, pi)."""
    angle_rad = angle_rad % (2.0 * np.pi)
    if angle_rad >= np.pi:
        angle_rad -= 2.0 * np.pi
    return angle_rad

def assert_angles_close(angle1_rad: float, angle2_rad: float, tol_rad: float = ANGLE_TOLERANCE_RAD):
    diff = normalize_angle_rad(angle1_rad - angle2_rad)
    assert abs(diff) < tol_rad

@pytest.fixture
def earth_gravitational_parameter() -> Derived:
    return Earth.gravitational_parameter

@pytest.fixture
def gcrf() -> Frame:
    return Frame.GCRF()

@pytest.fixture
def default_instant() -> Instant:
    return Instant.J2000()

@pytest.fixture
def meoe_default_values():
    return {
        "p": Length.kilometers(7500.0),
        "f": Real(0.01),
        "g": Real(0.02),
        "h": Real(0.03),
        "k": Real(0.04),
        "L": Angle.degrees(45.0)
    }

@pytest.fixture
def meoe_object(meoe_default_values) -> MEOE:
    return MEOE(
        meoe_default_values["p"],
        meoe_default_values["f"],
        meoe_default_values["g"],
        meoe_default_values["h"],
        meoe_default_values["k"],
        meoe_default_values["L"]
    )

class TestMEOE:

    def test_constructor_and_getters(self, meoe_object: MEOE, meoe_default_values):
        assert meoe_object.is_defined()
        assert meoe_object.get_semi_latus_rectum() == meoe_default_values["p"]
        assert meoe_object.get_f() == meoe_default_values["f"]
        assert meoe_object.get_g() == meoe_default_values["g"]
        assert meoe_object.get_h() == meoe_default_values["h"]
        assert meoe_object.get_k() == meoe_default_values["k"]
        assert meoe_object.get_true_longitude() == meoe_default_values["L"]

    def test_undefined_state(self):
        meoe_undefined = MEOE.undefined()
        assert not meoe_undefined.is_defined()

        with pytest.raises(RuntimeError):
            meoe_undefined.get_semi_latus_rectum()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_f()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_g()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_h()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_k()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_true_longitude()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_si_vector()
        with pytest.raises(RuntimeError):
            meoe_undefined.get_cartesian_state(Earth.gravitational_parameter, Frame.GCRF())


    def test_operators(self, meoe_object: MEOE, meoe_default_values):
        meoe_copy = MEOE(
            meoe_default_values["p"], meoe_default_values["f"], meoe_default_values["g"],
            meoe_default_values["h"], meoe_default_values["k"], meoe_default_values["L"]
        )
        meoe_different_p = MEOE(
            Length.kilometers(7501.0), meoe_default_values["f"], meoe_default_values["g"],
            meoe_default_values["h"], meoe_default_values["k"], meoe_default_values["L"]
        )

        assert meoe_object == meoe_copy
        assert meoe_object != meoe_different_p
        assert (meoe_object == MEOE.undefined()) is False
        assert (MEOE.undefined() == MEOE.undefined()) is False # Undefined is not equal to undefined


    def test_string_representations(self, meoe_object: MEOE, meoe_default_values):
        assert str(meoe_object) == f"MEOE (p: {meoe_default_values['p'].to_string()}, f: {meoe_default_values['f']:.6f}, g: {meoe_default_values['g']:.6f}, h: {meoe_default_values['h']:.6f}, k: {meoe_default_values['k']:.6f}, L: {meoe_default_values['L'].to_string(Angle.Unit.Degree)} [deg])" # Comparing with decorator

        repr_str = repr(meoe_object)
        assert "MEOE(p=" in repr_str
        assert f"f={meoe_default_values['f']}" in repr_str
        assert f"L={meoe_default_values['L'].to_string()}" in repr_str
        assert repr(MEOE.undefined()) == "MEOE.undefined()"


    def test_get_si_vector(self, meoe_object: MEOE, meoe_default_values):
        si_vector = meoe_object.get_si_vector()
        assert isinstance(si_vector, np.ndarray)
        assert len(si_vector) == 6
        assert si_vector[0] == pytest.approx(meoe_default_values["p"].in_meters(), abs=POSITION_TOLERANCE_M)
        assert si_vector[1] == pytest.approx(meoe_default_values["f"], abs=MEOE_ELEMENT_TOLERANCE)
        assert si_vector[2] == pytest.approx(meoe_default_values["g"], abs=MEOE_ELEMENT_TOLERANCE)
        assert si_vector[3] == pytest.approx(meoe_default_values["h"], abs=MEOE_ELEMENT_TOLERANCE)
        assert si_vector[4] == pytest.approx(meoe_default_values["k"], abs=MEOE_ELEMENT_TOLERANCE)
        assert si_vector[5] == pytest.approx(meoe_default_values["L"].in_radians(), abs=ANGLE_TOLERANCE_RAD)

    def test_cartesian_to_meoe_general_elliptical(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        # Reference COE (Example from Vallado, similar to GTO)
        sma_ref = Length.kilometers(24396.137)
        ecc_ref = Real(0.7308)
        inc_ref = Angle.degrees(7.0)
        raan_ref = Angle.degrees(45.0)
        aop_ref = Angle.degrees(90.0)
        ta_ref = Angle.degrees(0.0)

        coe_ref = COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)
        cartesian_state_ref = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)

        meoe = MEOE.cartesian(cartesian_state_ref, earth_gravitational_parameter)
        assert meoe.is_defined()

        # Expected MEOE values
        p_expected_m = sma_ref.in_meters() * (1.0 - ecc_ref * ecc_ref)
        f_expected = ecc_ref * np.cos(aop_ref.in_radians() + raan_ref.in_radians())
        g_expected = ecc_ref * np.sin(aop_ref.in_radians() + raan_ref.in_radians())
        tan_i_half_expected = np.tan(inc_ref.in_radians() / 2.0)
        h_expected = tan_i_half_expected * np.cos(raan_ref.in_radians())
        k_expected = tan_i_half_expected * np.sin(raan_ref.in_radians())
        L_expected_rad = normalize_angle_rad(raan_ref.in_radians() + aop_ref.in_radians() + ta_ref.in_radians())

        assert meoe.get_semi_latus_rectum().in_meters() == pytest.approx(p_expected_m, abs=POSITION_TOLERANCE_M)
        assert meoe.get_f() == pytest.approx(f_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_g() == pytest.approx(g_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_h() == pytest.approx(h_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_k() == pytest.approx(k_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert_angles_close(meoe.get_true_longitude().in_radians(), L_expected_rad)


    def test_meoe_to_cartesian_general(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        # MEOE: p = 11263.5 km, f = -0.51675, g = 0.51675, h = 0.04305, k = 0.04305, L = 135.0 deg
        # Corresponds to COE: a = 24396.137 km, e = 0.7308, i = 7.0 deg, RAAN = 45.0 deg, AOP = 90.0 deg, TA = 0.0 deg
        p_meoe = Length.kilometers(11263.51934)
        f_meoe = Real(-0.51675014)
        g_meoe = Real(0.51675014)
        h_meoe = Real(np.tan(Angle.degrees(7.0).in_radians()/2.0) * np.cos(Angle.degrees(45.0).in_radians()))
        k_meoe = Real(np.tan(Angle.degrees(7.0).in_radians()/2.0) * np.sin(Angle.degrees(45.0).in_radians()))
        L_meoe = Angle.degrees(135.0)

        meoe = MEOE(p_meoe, f_meoe, g_meoe, h_meoe, k_meoe, L_meoe)

        # Expected Cartesian from COE
        coe_ref = COE(Length.kilometers(24396.137), Real(0.7308), Angle.degrees(7.0), Angle.degrees(45.0), Angle.degrees(90.0), Angle.degrees(0.0))
        cartesian_state_expected = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)
        pos_expected_gcrf = cartesian_state_expected.get_position().get_coordinates()
        vel_expected_gcrf = cartesian_state_expected.get_velocity().get_coordinates()

        cartesian_state_actual = meoe.get_cartesian_state(earth_gravitational_parameter, gcrf)
        pos_actual_gcrf = cartesian_state_actual.get_position().get_coordinates()
        vel_actual_gcrf = cartesian_state_actual.get_velocity().get_coordinates()

        assert np.allclose(pos_actual_gcrf, pos_expected_gcrf, atol=POSITION_TOLERANCE_M)
        assert np.allclose(vel_actual_gcrf, vel_expected_gcrf, atol=VELOCITY_TOLERANCE_MPS)

    def test_round_trip_cartesian_meoe_cartesian(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        sma_initial = Length.kilometers(20000.0)
        ecc_initial = Real(0.6)
        inc_initial = Angle.degrees(25.0)
        raan_initial = Angle.degrees(50.0)
        aop_initial = Angle.degrees(75.0)
        ta_initial = Angle.degrees(100.0)

        coe_initial = COE(sma_initial, ecc_initial, inc_initial, raan_initial, aop_initial, ta_initial)
        cartesian_initial = coe_initial.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)
        pos_initial_gcrf = cartesian_initial.get_position().get_coordinates()
        vel_initial_gcrf = cartesian_initial.get_velocity().get_coordinates()

        meoe_intermediate = MEOE.cartesian(cartesian_initial, earth_gravitational_parameter)
        assert meoe_intermediate.is_defined()

        cartesian_final = meoe_intermediate.get_cartesian_state(earth_gravitational_parameter, gcrf)
        pos_final_gcrf = cartesian_final.get_position().get_coordinates()
        vel_final_gcrf = cartesian_final.get_velocity().get_coordinates()

        assert np.allclose(pos_final_gcrf, pos_initial_gcrf, atol=POSITION_TOLERANCE_M)
        assert np.allclose(vel_final_gcrf, vel_initial_gcrf, atol=VELOCITY_TOLERANCE_MPS)


    def test_round_trip_meoe_cartesian_meoe(self, earth_gravitational_parameter: Derived, gcrf: Frame):
        p_initial = Length.kilometers(10000.0)
        f_initial = Real(0.1)
        g_initial = Real(-0.05)
        h_initial = Real(0.2)
        k_initial = Real(-0.15)
        L_initial = Angle.degrees(120.0)

        meoe_initial = MEOE(p_initial, f_initial, g_initial, h_initial, k_initial, L_initial)

        cartesian_intermediate = meoe_initial.get_cartesian_state(earth_gravitational_parameter, gcrf)
        assert cartesian_intermediate.get_position().is_defined()
        assert cartesian_intermediate.get_velocity().is_defined()

        meoe_final = MEOE.cartesian(cartesian_intermediate, earth_gravitational_parameter)
        assert meoe_final.is_defined()

        assert meoe_final.get_semi_latus_rectum().in_meters() == pytest.approx(p_initial.in_meters(), abs=POSITION_TOLERANCE_M)
        assert meoe_final.get_f() == pytest.approx(f_initial, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_final.get_g() == pytest.approx(g_initial, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_final.get_h() == pytest.approx(h_initial, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_final.get_k() == pytest.approx(k_initial, abs=MEOE_ELEMENT_TOLERANCE)
        assert_angles_close(meoe_final.get_true_longitude().in_radians(), L_initial.in_radians())


    def test_cartesian_to_meoe_inclination_pi_singularity(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        sma_ref = Length.kilometers(10000.0)
        ecc_ref = Real(0.1)
        inc_ref = Angle.radians(np.pi) # 180 degrees
        raan_ref = Angle.degrees(0.0)
        aop_ref = Angle.degrees(0.0)
        ta_ref = Angle.degrees(0.0)

        coe_ref = COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)
        cartesian_state_ref = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)

        meoe = MEOE.cartesian(cartesian_state_ref, earth_gravitational_parameter)
        assert not meoe.is_defined()

    def test_meoe_to_cartesian_invalid_p(self, earth_gravitational_parameter: Derived, gcrf: Frame):
        meoe_invalid_p_zero = MEOE(Length.meters(0.0), Real(0.1), Real(0.1), Real(0.1), Real(0.1), Angle.degrees(0.0))
        with pytest.raises(RuntimeError): # Matches C++ exception ostk::core::error::RuntimeError
            meoe_invalid_p_zero.get_cartesian_state(earth_gravitational_parameter, gcrf)

        meoe_invalid_p_neg = MEOE(Length.meters(-100.0), Real(0.1), Real(0.1), Real(0.1), Real(0.1), Angle.degrees(0.0))
        with pytest.raises(RuntimeError):
            meoe_invalid_p_neg.get_cartesian_state(earth_gravitational_parameter, gcrf)

    def test_meoe_to_cartesian_q_zero_singularity(self, earth_gravitational_parameter: Derived, gcrf: Frame):
        # q = 1 + f*cosL + g*sinL. If q = 0, singularity.
        # Example: f = -1, L = 0 => q = 1 + (-1)*1 + 0*0 = 0
        meoe_q_zero = MEOE(Length.kilometers(7000.0), Real(-1.0), Real(0.0), Real(0.0), Real(0.0), Angle.radians(0.0))
        with pytest.raises(RuntimeError):
            meoe_q_zero.get_cartesian_state(earth_gravitational_parameter, gcrf)

    def test_pair_output_get_cartesian_state(self, meoe_object: MEOE, earth_gravitational_parameter: Derived, gcrf: Frame):
        # Test that the output is indeed a Pair[Position, Velocity]
        # The binding for Pair should handle this automatically if it's exposed correctly
        # In Python, this will likely be a tuple (Position, Velocity)
        cartesian_state = meoe_object.get_cartesian_state(earth_gravitational_parameter, gcrf)
        assert isinstance(cartesian_state, tuple) # pybind11 converts std::pair to python tuple
        assert len(cartesian_state) == 2
        assert isinstance(cartesian_state[0], Position)
        assert isinstance(cartesian_state[1], Velocity)

    def test_pair_input_cartesian(self, meoe_object: MEOE, earth_gravitational_parameter: Derived, gcrf: Frame):
        # Test that MEOE.cartesian accepts a tuple (Position, Velocity)
        # This is the reverse of the above test
        cartesian_state_tuple = meoe_object.get_cartesian_state(earth_gravitational_parameter, gcrf)

        # Construct MEOE from this tuple
        meoe_from_tuple = MEOE.cartesian(cartesian_state_tuple, earth_gravitational_parameter)
        assert meoe_from_tuple.is_defined()

        # Compare with original MEOE (within tolerance)
        assert meoe_from_tuple.get_semi_latus_rectum().in_meters() == pytest.approx(meoe_object.get_semi_latus_rectum().in_meters(), abs=POSITION_TOLERANCE_M)
        assert meoe_from_tuple.get_f() == pytest.approx(meoe_object.get_f(), abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_from_tuple.get_g() == pytest.approx(meoe_object.get_g(), abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_from_tuple.get_h() == pytest.approx(meoe_object.get_h(), abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe_from_tuple.get_k() == pytest.approx(meoe_object.get_k(), abs=MEOE_ELEMENT_TOLERANCE)
        assert_angles_close(meoe_from_tuple.get_true_longitude().in_radians(), meoe_object.get_true_longitude().in_radians())

        # Also test with ostk.core.container.Pair if it's directly exposed and usable (less common for input)
        # pair_obj = Pair[Position, Velocity](cartesian_state_tuple[0], cartesian_state_tuple[1]) # This syntax might not be directly available
        # meoe_from_pair_obj = MEOE.cartesian(pair_obj, earth_gravitational_parameter)
        # assert meoe_from_pair_obj.is_defined()
        # For now, tuple input is the primary concern as that's pybind11's default for std::pair.

    def test_str_method_for_undefined(self):
        # Check that str() on an undefined MEOE doesn't crash and gives a reasonable representation
        meoe_undefined = MEOE.undefined()
        str_out = str(meoe_undefined)
        assert "Undef" in str_out or "undefined" in str_out.lower() # Check for Undefined representation
        assert "MEOE" in str_out

    def test_default_constructor_not_exposed(self): # MEOE has no default C++ constructor
        with pytest.raises(TypeError): # Expect a TypeError if trying to call MEOE() with no args
            MEOE()

    # Example of a circular orbit
    def test_cartesian_to_meoe_circular(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        sma_ref = Length.kilometers(7000.0)
        ecc_ref = Real(0.0) # Circular
        inc_ref = Angle.degrees(28.5)
        raan_ref = Angle.degrees(10.0)
        aop_ref = Angle.degrees(0.0) # Typically undefined for circular, set to 0 for COE
        ta_ref = Angle.degrees(45.0) # Argument of latitude

        coe_ref = COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)
        cartesian_state_ref = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)

        meoe = MEOE.cartesian(cartesian_state_ref, earth_gravitational_parameter)
        assert meoe.is_defined()

        assert meoe.get_semi_latus_rectum().in_meters() == pytest.approx(sma_ref.in_meters(), abs=POSITION_TOLERANCE_M) # p = a for circular
        assert meoe.get_f() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_g() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)

        tan_i_half_expected = np.tan(inc_ref.in_radians() / 2.0)
        h_expected = tan_i_half_expected * np.cos(raan_ref.in_radians())
        k_expected = tan_i_half_expected * np.sin(raan_ref.in_radians())
        # L = RAAN + AOP + TA. For circular, AOP might be 0 from COE, so L = RAAN + TA
        L_expected_rad = normalize_angle_rad(raan_ref.in_radians() + aop_ref.in_radians() + ta_ref.in_radians())

        assert meoe.get_h() == pytest.approx(h_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_k() == pytest.approx(k_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert_angles_close(meoe.get_true_longitude().in_radians(), L_expected_rad)

    # Example of an equatorial orbit
    def test_cartesian_to_meoe_equatorial(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        sma_ref = Length.kilometers(7000.0)
        ecc_ref = Real(0.05)
        inc_ref = Angle.degrees(0.0) # Equatorial
        raan_ref = Angle.degrees(0.0) # Typically undefined, set to 0
        aop_ref = Angle.degrees(30.0) # True longitude of periapsis
        ta_ref = Angle.degrees(15.0)

        coe_ref = COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)
        cartesian_state_ref = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)

        meoe = MEOE.cartesian(cartesian_state_ref, earth_gravitational_parameter)
        assert meoe.is_defined()

        p_expected_m = sma_ref.in_meters() * (1.0 - ecc_ref * ecc_ref)
        f_expected = ecc_ref * np.cos(aop_ref.in_radians() + raan_ref.in_radians()) # raan=0
        g_expected = ecc_ref * np.sin(aop_ref.in_radians() + raan_ref.in_radians()) # raan=0
        L_expected_rad = normalize_angle_rad(raan_ref.in_radians() + aop_ref.in_radians() + ta_ref.in_radians()) # True longitude

        assert meoe.get_semi_latus_rectum().in_meters() == pytest.approx(p_expected_m, abs=POSITION_TOLERANCE_M)
        assert meoe.get_f() == pytest.approx(f_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_g() == pytest.approx(g_expected, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_h() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE) # h = 0 for i=0
        assert meoe.get_k() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE) # k = 0 for i=0
        assert_angles_close(meoe.get_true_longitude().in_radians(), L_expected_rad)

    # Example of a circular equatorial orbit
    def test_cartesian_to_meoe_circular_equatorial(self, earth_gravitational_parameter: Derived, gcrf: Frame, default_instant: Instant):
        sma_ref = Length.kilometers(42164.0) # GEO-like
        ecc_ref = Real(0.0)
        inc_ref = Angle.degrees(0.0)
        raan_ref = Angle.degrees(0.0) # Undefined, set to 0
        aop_ref = Angle.degrees(0.0)  # Undefined, set to 0
        ta_ref = Angle.degrees(75.0)  # True longitude

        coe_ref = COE(sma_ref, ecc_ref, inc_ref, raan_ref, aop_ref, ta_ref)
        cartesian_state_ref = coe_ref.get_cartesian_state(earth_gravitational_parameter, gcrf, default_instant)

        meoe = MEOE.cartesian(cartesian_state_ref, earth_gravitational_parameter)
        assert meoe.is_defined()

        assert meoe.get_semi_latus_rectum().in_meters() == pytest.approx(sma_ref.in_meters(), abs=POSITION_TOLERANCE_M)
        assert meoe.get_f() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_g() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_h() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)
        assert meoe.get_k() == pytest.approx(0.0, abs=MEOE_ELEMENT_TOLERANCE)
        # L = RAAN + AOP + TA. With RAAN=0, AOP=0, L = TA (True Longitude)
        assert_angles_close(meoe.get_true_longitude().in_radians(), ta_ref.in_radians())

    def test_meoe_constructor_units(self):
        # Ensure constructor correctly handles different units for p and L
        p_km = Length.kilometers(7000.0)
        L_deg = Angle.degrees(90.0)
        meoe = MEOE(p_km, Real(0.1), Real(0.1), Real(0.1), Real(0.1), L_deg)

        assert meoe.get_semi_latus_rectum().in_meters() == pytest.approx(7000000.0)
        assert meoe.get_true_longitude().in_radians() == pytest.approx(np.pi / 2.0)

        p_m = Length.meters(8000000.0)
        L_rad = Angle.radians(np.pi / 4.0)
        meoe2 = MEOE(p_m, Real(0.1), Real(0.1), Real(0.1), Real(0.1), L_rad)

        assert meoe2.get_semi_latus_rectum().in_kilometers() == pytest.approx(8000.0)
        assert meoe2.get_true_longitude().in_degrees() == pytest.approx(45.0)
