# Apache License 2.0

import pytest

from ostk.mathematics.objects import RealInterval

from ostk.physics.units import Length
from ostk.physics.units import Angle
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics import Environment
from ostk.physics.environment.objects import Celestial

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.models import Kepler
from ostk.astrodynamics.trajectory.orbit.models.kepler import COE
from ostk.astrodynamics import Access
from ostk.astrodynamics.access import Generator


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


@pytest.fixture
def generator(environment: Environment) -> Generator:
    return Generator(
        environment=environment,
        aer_filter=lambda aer: True,
        access_filter=lambda access: True,
        state_filter=lambda state_1, state_2: True,
    )


@pytest.fixture
def from_trajectory(earth: Celestial) -> Trajectory:
    return Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(45.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(0.0),
            ),
            epoch=Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )


@pytest.fixture
def to_trajectory(earth: Celestial) -> Trajectory:
    return Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(45.0),
                raan=Angle.degrees(180.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(180.0),
            ),
            epoch=Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )


class TestGenerator:
    def test_constructor_success_environment(self, environment: Environment):
        generator = Generator(
            environment=environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_aer_filter(self, environment: Environment):
        generator = Generator(
            environment=environment,
            aer_filter=lambda aer: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_access_filter(
        self,
        environment: Environment,
    ):
        generator = Generator(
            environment=environment,
            access_filter=lambda access: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_state_filter(
        self,
        environment: Environment,
    ):
        generator = Generator(
            environment=environment,
            state_filter=lambda state_1, state_2: True,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_success_environment_step_tolerance(
        self,
        environment: Environment,
    ):
        generator = Generator(
            environment=environment,
            step=Duration.seconds(1.0),
            tolerance=Duration.minutes(1.0),
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.get_step() == Duration.seconds(1.0)
        assert generator.get_tolerance() == Duration.minutes(1.0)

    def test_getters_success(self, generator: Generator):
        assert generator.get_step() == Duration.minutes(1.0)
        assert generator.get_tolerance() == Duration.microseconds(1.0)
        assert generator.get_aer_filter() is not None
        assert generator.get_access_filter() is not None
        assert generator.get_state_filter() is not None

    def test_get_condition_function_success(
        self,
        generator: Generator,
        from_trajectory: Trajectory,
        to_trajectory: Trajectory,
    ):
        condition_function = generator.get_condition_function(
            from_trajectory=from_trajectory,
            to_trajectory=to_trajectory,
        )

        assert condition_function is not None
        assert (
            condition_function(
                Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
            )
            is True
        )

    def test_compute_accesses_success(
        self,
        generator: Generator,
        from_trajectory: Trajectory,
        to_trajectory: Trajectory,
    ):
        accesses = generator.compute_accesses(
            interval=Interval.closed(
                Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
                Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC),
            ),
            from_trajectory=from_trajectory,
            to_trajectory=to_trajectory,
        )

        assert accesses is not None
        assert isinstance(accesses, list)
        assert accesses[0] is not None
        assert isinstance(accesses[0], Access)

    def test_set_step_success(self, generator: Generator):
        generator.set_step(Duration.seconds(1.0))

    def test_set_tolerance_success(self, generator: Generator):
        generator.set_tolerance(Duration.seconds(1.0))

    def test_set_aer_filter_success(self, generator: Generator):
        generator.set_aer_filter(aer_filter=lambda aer: True)

    def test_set_access_filter_success(self, generator: Generator):
        generator.set_access_filter(access_filter=lambda access: True)

    def test_set_state_filter_success(self, generator: Generator):
        generator.set_state_filter(state_filter=lambda state_1, state_2: True)

    def test_undefined_success(self):
        generator = Generator.undefined()

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is False

    def test_aer_ranges_success(self, environment: Environment):
        # Construct arbitrary AER ranges
        azimuth_interval = RealInterval.closed(0.0, 360.0)
        elevation_interval = RealInterval.closed(0.0, 90.0)
        range_interval = RealInterval.closed(0.0, 7000e3)

        generator = Generator.aer_ranges(
            azimuth_range=azimuth_interval,
            elevation_range=elevation_interval,
            range_range=range_interval,
            environment=environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined()

    def test_aer_mask_success(self, environment: Environment):
        # Construct arbitrary anAzimuthElevationMask using python dict
        an_azimuth_elevation_mask = {
            0.0: 30.0,
            90.0: 60.0,
            180.0: 60.0,
            270.0: 30.0,
            359.0: 30.0,
        }

        # Construct arbitrary aRangerange
        a_range_range = RealInterval(0.0, 10e4, RealInterval.Type.Closed)

        generator = Generator.aer_mask(
            azimuth_elevation_mask=an_azimuth_elevation_mask,
            range_range=a_range_range,
            environment=environment,
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined()
