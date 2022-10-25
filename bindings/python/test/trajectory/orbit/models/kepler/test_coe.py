################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_coe.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Length = physics.units.Length
Angle = physics.units.Angle
Scale = physics.time.Scale
Instant = physics.time.Instant
Interval = physics.time.Interval
DateTime = physics.time.DateTime
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Environment = physics.Environment

Trajectory = astrodynamics.Trajectory
Model = astrodynamics.trajectory.Model
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.SGP4
TLE = astrodynamics.trajectory.orbit.models.sgp4.TLE
State = astrodynamics.trajectory.State
Access = astrodynamics.Access

environment = Environment.default()
earth = environment.access_celestial_object_with_name("Earth")

################################################################################################################################################################

def construct_coe ():

    a = Length.kilometers(7000.0)
    e = 0.1
    i = Angle.degrees(35.0)
    raan = Angle.degrees(40.0)
    aop = Angle.degrees(50.0)
    nu = Angle.degrees(60.0)

    coe = COE(a, e, i, raan, aop, nu)

    return coe

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_constructors ():

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

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_comparators ():

    coe: COE = construct_coe()

    assert coe == coe
    assert (coe != coe) is False

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_getters ():

    coe: COE = construct_coe()

    # get_semi_major_axis()

    assert coe.get_semi_major_axis() is not None

    # get_eccentricity()

    assert coe.get_eccentricity() is not None

    # get_inclination()

    assert coe.get_inclination() is not None

    # get_raan()

    assert coe.get_raan() is not None

    # get_aop()

    assert coe.get_aop() is not None

    # get_true_anomaly()

    assert coe.get_true_anomaly() is not None

    # get_mean_anomaly()

    assert coe.get_mean_anomaly() is not None

    # get_eccentric_anomaly()

    assert coe.get_eccentric_anomaly() is not None

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_get_mean_motion ():

    coe: COE = construct_coe()

    assert coe.get_mean_motion(earth.gravitational_parameter) is not None

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_get_orbital_period ():

    coe: COE = construct_coe()

    assert coe.get_orbital_period(earth.gravitational_parameter) is not None

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_coe_static_methods ():

    coe: COE = construct_coe()

    assert COE.eccentric_anomaly_from_true_anomaly(Angle.degrees(0.0), 0.0) is not None
    assert COE.true_anomaly_from_eccentric_anomaly(Angle.degrees(0.0), 0.0) is not None
    assert COE.mean_anomaly_from_eccentric_anomaly(Angle.degrees(0.0), 0.0) is not None
    assert COE.eccentric_anomaly_from_mean_anomaly(Angle.degrees(0.0), 0.0, 0.0) is not None

################################################################################################################################################################



################################################################################################################################################################



################################################################################################################################################################
