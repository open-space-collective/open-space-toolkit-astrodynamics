################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/access/test_generator.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest
import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

RealInterval = mathematics.objects.RealInterval
Quaternion = mathematics.geometry.d3.transformations.rotations.Quaternion
Length = physics.units.Length
Angle = physics.units.Angle
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Duration = physics.time.Duration
Instant = physics.time.Instant
Interval = physics.time.Interval
Transform = physics.coordinate.Transform
Frame = physics.coordinate.Frame
Axes = physics.coordinate.Axes
DynamicProvider = physics.coordinate.frame.providers.Dynamic
Environment = physics.Environment
Earth = physics.environment.objects.celestial_bodies.Earth
Trajectory = astrodynamics.Trajectory
Profile = astrodynamics.flight.Profile
State = astrodynamics.flight.profile.State
Orbit = astrodynamics.trajectory.Orbit
Pass = astrodynamics.trajectory.orbit.Pass
Kepler = astrodynamics.trajectory.orbit.models.Kepler
COE = astrodynamics.trajectory.orbit.models.kepler.COE
SGP4 = astrodynamics.trajectory.orbit.models.sgp4
Access = astrodynamics.Access
Generator = astrodynamics.access.Generator

environment: Environment = Environment.default()

################################################################################################################################################################

@pytest.fixture
def generator () -> Generator:

    return Generator(environment)

################################################################################################################################################################

class TestGenerator:

    def test_constructor_success_environment (self):

        generator = Generator(
            environment = environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_aer_filter (self):

        generator = Generator(
            environment = environment,
            aer_filter = lambda aer: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_access_filter (self):

        generator = Generator(
            environment = environment,
            access_filter = lambda access: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_state_filter (self):

        generator = Generator(
            environment = environment,
            state_filter = lambda state_1, state_2: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_step_tolerance (self):

        generator = Generator(
            environment = environment,
            step = Duration.seconds(1.0),
            tolerance = Duration.minutes(1.0),
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.get_step() == Duration.seconds(1.0)
        assert generator.get_tolerance() == Duration.minutes(1.0)

    def test_getters_success (self, generator: Generator):

        assert generator.get_step() == Duration.minutes(1.0)
        assert generator.get_tolerance() == Duration.microseconds(1.0)

    def test_compute_accesses_success (self):

        # Contruction with environment and no filters

        generator = Generator(environment)

        start_instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
        end_instant = Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC)
        interval = Interval.closed(start_instant, end_instant)

        def generate_first_trajectory ():

            a = Length.kilometers(7000.0)
            e = 0.0
            i = Angle.degrees(45.0)
            raan = Angle.degrees(0.0)
            aop = Angle.degrees(0.0)
            nu = Angle.degrees(0.0)

            coe = COE(a, e, i, raan, aop, nu)

            epoch = start_instant
            earth = Earth.default()

            kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

            return Orbit(kepler, earth)

        def generate_second_trajectory ():

            a = Length.kilometers(7000.0)
            e = 0.0
            i = Angle.degrees(45.0)
            raan = Angle.degrees(180.0)
            aop = Angle.degrees(0.0)
            nu = Angle.degrees(180.0)

            coe = COE(a, e, i, raan, aop, nu)

            epoch = start_instant
            earth = Earth.default()

            kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)

            return Orbit(kepler, earth)

        from_trajectory = generate_first_trajectory()
        to_trajectory = generate_second_trajectory()

        accesses = generator.compute_accesses(
            interval = interval,
            from_trajectory = from_trajectory,
            to_trajectory = to_trajectory,
        )

        assert accesses is not None
        assert isinstance(accesses, list)
        assert accesses[0] is not None
        assert isinstance(accesses[0], Access)

    def test_set_step_success (self, generator: Generator):

        generator.set_step(Duration.seconds(1.0))

    def test_set_tolerance_success (self, generator: Generator):

        generator.set_tolerance(Duration.seconds(1.0))

    def test_set_aer_filter_success (self, generator: Generator):

        generator.set_aer_filter(aer_filter = lambda aer: True)

    def test_set_access_filter_success (self, generator: Generator):

        generator.set_access_filter(access_filter = lambda access: True)

    def test_set_state_filter_success (self, generator: Generator):

        generator.set_state_filter(state_filter = lambda state_1, state_2: True)

    def test_undefined_success (self):

        generator = Generator.undefined()

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is False

    def test_aer_ranges_success (self):

        # Construct arbitrary AER ranges
        azimuth_interval = RealInterval.closed(0.0, 360.0)
        elevation_interval = RealInterval.closed(0.0, 90.0)
        range_interval = RealInterval.closed(0.0, 7000e3)

        generator = Generator.aer_ranges(
            azimuth_range = azimuth_interval,
            elevation_range = elevation_interval,
            range_range = range_interval,
            environment = environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined()

    def test_aer_mask_success (self):

        # Construct arbitrary anAzimuthElevationMask using python dict
        an_azimuth_elevation_mask = {0.0: 30.0, 90.0: 60.0, 180.0: 60.0, 270.0: 30.0, 359.0: 30.0}

        # Construct arbitrary aRangerange
        a_range_range = RealInterval(0.0, 10e4, RealInterval.Type.Closed)

        generator = Generator.aer_mask(
            azimuth_elevation_mask = an_azimuth_elevation_mask,
            range_range = a_range_range,
            environment = environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined()

################################################################################################################################################################
