################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_kepler.py
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

earth = Environment.default().access_celestial_object_with_name("Earth")

################################################################################################################################################################

def construct_kepler ():

    a = Length.kilometers(7000.0)
    e = 0.1
    i = Angle.degrees(35.0)
    raan = Angle.degrees(40.0)
    aop = Angle.degrees(50.0)
    nu = Angle.degrees(60.0)

    coe = COE(a, e, i, raan, aop, nu)

    assert coe is not None
    assert isinstance(coe, COE)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

    return kepler

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_constructors ():

    a = Length.kilometers(7000.0)
    e = 0.1
    i = Angle.degrees(35.0)
    raan = Angle.degrees(40.0)
    aop = Angle.degrees(50.0)
    nu = Angle.degrees(60.0)

    coe = COE(a, e, i, raan, aop, nu)

    assert coe is not None
    assert isinstance(coe, COE)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

    assert kepler is not None
    assert isinstance(kepler, Kepler)
    assert kepler.is_defined()

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_comparators ():

    kepler: Kepler = construct_kepler()

    assert kepler == kepler
    assert (kepler != kepler) is False

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_getters ():

    kepler: Kepler = construct_kepler()

    # get_classical_orbital_elements

    assert kepler.get_classical_orbital_elements() is not None

    # get_epoch

    assert kepler.get_epoch() is not None

    # get_revolution_number_at_epoch()

    assert kepler.get_revolution_number_at_epoch() is not None

    # get_gravitational_parameter()

    assert kepler.get_gravitational_parameter() is not None

    # get_equatorial_radius()

    assert kepler.get_equatorial_radius() is not None

    # get_j2()

    assert kepler.get_j2() is not None

    # get_j4()

    assert kepler.get_j4() is not None

    # get_perturbation_type()

    assert kepler.get_perturbation_type() is not None

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_calculate_state_at_epoch ():

    kepler: Kepler = construct_kepler()

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    assert kepler.calculate_state_at(epoch) is not None

################################################################################################################################################################

def test_trajectory_orbit_models_kepler_calculate_rev_number_at_epoch ():

    kepler: Kepler = construct_kepler()

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    assert kepler.calculate_revolution_number_at(epoch) is not None

################################################################################################################################################################
