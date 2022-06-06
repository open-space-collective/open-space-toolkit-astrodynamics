################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/models/test_propagated.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import ostk.physics as physics
import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Length = physics.units.Length
Derived = physics.units.Derived
Angle = physics.units.Angle
Time = physics.units.Time
Scale = physics.time.Scale
Instant = physics.time.Instant
Interval = physics.time.Interval
DateTime = physics.time.DateTime
Position = physics.coordinate.Position
Velocity = physics.coordinate.Velocity
Frame = physics.coordinate.Frame
Unit = physics.units.Length.Unit
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth

Trajectory = astrodynamics.Trajectory
Model = astrodynamics.trajectory.Model
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Propagated = astrodynamics.trajectory.orbit.models.Propagated

State = astrodynamics.trajectory.State
Access = astrodynamics.Access

earth_env = Environment.default().access_celestial_object_with_name("Earth")

################################################################################################################################################################

def construct_propagated ():


    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([6371000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)
    
    state = State(epoch, position, velocity)

    assert state is not None
    assert isinstance(state, State)
    
    propagated = Propagated(state, epoch, Earth.gravitational_parameter, Earth.equatorial_radius, Propagated.GravitationalPerturbationType.No, Propagated.AtmosphericPerturbationType.No, Propagated.ThirdBodyPerturbationType.No, False)
    
    assert propagated is not None
    assert isinstance(propagated, Propagated)
    assert propagated.is_defined()

    return propagated

################################################################################################################################################################

def test_trajectory_orbit_models_propagated_constructors ():

    propagated: Propagated = construct_propagated()

    print(propagated)

    assert propagated is not None
    assert isinstance(propagated, Propagated)
    assert propagated.is_defined()

# ################################################################################################################################################################

def test_trajectory_orbit_models_propagated_comparators ():

    propagated: Propagated = construct_propagated()

    assert propagated == propagated
    assert (propagated != propagated) is False

# ################################################################################################################################################################

def test_trajectory_orbit_models_propagated_getters ():

    propagated: Propagated = construct_propagated()

    # get_epoch

    assert propagated.get_epoch() is not None

    # get_revolution_number_at_epoch()

    assert propagated.get_revolution_number_at_epoch() is not None

    # get_gravitational_parameter()

    assert propagated.get_gravitational_parameter() is not None

    # get_equatorial_radius()

    assert propagated.get_equatorial_radius() is not None

    # get_gravitational_perturbation_type() 

    assert propagated.get_gravitational_perturbation_type() is not None

    # get_atmospheric_perturbation_type() 

    assert propagated.get_atmospheric_perturbation_type() is not None

    # get_thirdbody_perturbation_type() 

    assert propagated.get_thirdbody_perturbation_type() is not None

# ################################################################################################################################################################

def test_trajectory_orbit_models_propagated_calculate_state_at_epoch ():

    propagated: Propagated = construct_propagated()

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    assert propagated.calculate_state_at(epoch) is not None

# ################################################################################################################################################################

def test_trajectory_orbit_models_propagated_calculate_rev_number_at_epoch ():

    propagated: Propagated = construct_propagated()

    epoch: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    assert propagated.calculate_revolution_number_at(epoch) is not None

# ################################################################################################################################################################
