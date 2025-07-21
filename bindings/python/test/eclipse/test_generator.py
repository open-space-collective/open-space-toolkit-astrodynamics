# Apache License 2.0

import pytest

from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics import Environment
from ostk.physics.environment.object import Celestial
from ostk.physics.environment.utility import Eclipse
from ostk.physics.environment.utility import EclipsePhase

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.eclipse import Generator


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


@pytest.fixture
def search_step_size() -> Duration:
    return Duration.seconds(30.0)


@pytest.fixture
def search_tolerance() -> Duration:
    return Duration.milliseconds(10.0)


@pytest.fixture
def generator(
    environment: Environment,
    search_step_size: Duration,
    search_tolerance: Duration,
) -> Generator:
    return Generator(
        environment=environment,
        search_step_size=search_step_size,
        search_tolerance=search_tolerance,
    )


@pytest.fixture
def trajectory(earth: Celestial) -> Trajectory:
    return Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(6700.0),
                eccentricity=0.0,
                inclination=Angle.degrees(0.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(0.0),
            ),
            epoch=Instant.date_time(DateTime(2000, 1, 1, 0, 0, 0), Scale.UTC),
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )


@pytest.fixture
def analysis_interval() -> Interval:
    return Interval.closed(
        Instant.date_time(DateTime(2000, 1, 1, 14, 0, 0), Scale.UTC),
        Instant.date_time(DateTime(2000, 1, 1, 17, 0, 0), Scale.UTC),
    )


class TestGenerator:
    def test_constructor_and_properties_success(
        self,
        environment: Environment,
        search_step_size: Duration,
        search_tolerance: Duration,
    ):
        generator: Generator = Generator(
            environment=environment,
            search_step_size=search_step_size,
            search_tolerance=search_tolerance,
        )

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is True
        assert generator.get_environment().is_defined()
        assert generator.get_search_step_size() == search_step_size
        assert generator.get_search_tolerance() == search_tolerance

    def test_default_constructor_and_properties_success(self):
        generator: Generator = Generator()
        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is True

    def test_generate_success(
        self,
        generator: Generator,
        trajectory: Trajectory,
        analysis_interval: Interval,
    ):
        eclipses: list[Eclipse] = generator.generate(
            trajectory=trajectory,
            analysis_interval=analysis_interval,
            occulted_celestial_object_name="Sun",
            occulting_celestial_object_name="Earth",
        )

        assert eclipses is not None
        assert isinstance(eclipses, list)
        assert len(eclipses) > 0

        # Check that all returned objects are Eclipse instances
        for eclipse in eclipses:
            assert isinstance(eclipse, Eclipse)

            # Check eclipse phases
            phases: list[EclipsePhase] = eclipse.get_phases()
            assert isinstance(phases, list)
            assert len(phases) > 0

            for phase in phases:
                assert isinstance(phase, EclipsePhase)
