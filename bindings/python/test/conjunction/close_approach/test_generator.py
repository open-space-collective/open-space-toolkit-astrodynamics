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

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.conjunction import CloseApproach
from ostk.astrodynamics.conjunction.close_approach import Generator


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


@pytest.fixture
def epoch() -> Instant:
    return Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def step() -> Duration:
    return Duration.minutes(20.0)


@pytest.fixture
def tolerance() -> Duration:
    return Duration.milliseconds(1.0)


@pytest.fixture
def reference_trajectory(earth: Celestial, epoch: Instant) -> Trajectory:
    return Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(0.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(0.0),
            ),
            epoch=epoch,
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )


@pytest.fixture
def other_trajectory(earth: Celestial, epoch: Instant) -> Trajectory:
    return Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(90.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(1.0 - 3),  # Avoid co-location
            ),
            epoch=epoch,
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )


@pytest.fixture
def generator(
    reference_trajectory: Trajectory,
    step: Duration,
    tolerance: Duration,
) -> Generator:
    return Generator(
        reference_trajectory=reference_trajectory,
        step=step,
        tolerance=tolerance,
    )


@pytest.fixture
def search_interval(epoch: Instant) -> Interval:
    return Interval.closed(
        epoch,
        epoch + Duration.hours(3.0),
    )


class TestGenerator:
    def test_constructor_success(
        self,
        reference_trajectory: Trajectory,
        step: Duration,
        tolerance: Duration,
    ):
        generator = Generator(
            reference_trajectory=reference_trajectory,
            step=step,
            tolerance=tolerance,
        )

        assert generator is not None
        assert isinstance(generator, Generator)

    def test_constructor_with_defaults_success(
        self,
        reference_trajectory: Trajectory,
    ):
        generator = Generator(reference_trajectory=reference_trajectory)

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is True

    def test_is_defined_success(self, generator: Generator):
        assert generator.is_defined() is True

    def test_undefined_success(self):
        generator = Generator.undefined()

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is False

    def test_get_reference_trajectory_success(
        self,
        generator: Generator,
    ):
        trajectory = generator.get_reference_trajectory()

        assert trajectory is not None
        assert isinstance(trajectory, Trajectory)

    def test_get_step_success(
        self,
        generator: Generator,
        step: Duration,
    ):
        result_step = generator.get_step()

        assert result_step is not None
        assert isinstance(result_step, Duration)
        assert result_step == step

    def test_get_tolerance_success(
        self,
        generator: Generator,
        tolerance: Duration,
    ):
        result_tolerance = generator.get_tolerance()

        assert result_tolerance is not None
        assert isinstance(result_tolerance, Duration)
        assert result_tolerance == tolerance

    def test_set_step_success(self, generator: Generator):
        new_step = Duration.seconds(30.0)

        generator.set_step(step=new_step)

        assert generator.get_step() == new_step

    def test_set_tolerance_success(self, generator: Generator):
        new_tolerance = Duration.microseconds(100.0)

        generator.set_tolerance(tolerance=new_tolerance)

        assert generator.get_tolerance() == new_tolerance

    def test_compute_close_approaches_success(
        self,
        generator: Generator,
        other_trajectory: Trajectory,
        search_interval: Interval,
    ):
        close_approaches = generator.compute_close_approaches(
            trajectory=other_trajectory,
            search_interval=search_interval,
        )

        assert close_approaches is not None
        assert isinstance(close_approaches, list)
        assert len(close_approaches) > 1

        for close_approach in close_approaches:
            assert isinstance(close_approach, CloseApproach)
            assert close_approach.is_defined() is True
            assert search_interval.contains_instant(close_approach.get_instant())

    def test_compute_close_approaches_empty_interval_success(
        self,
        generator: Generator,
        other_trajectory: Trajectory,
        epoch: Instant,
    ):
        search_interval = Interval.closed(
            epoch + Duration.hours(10.0),
            epoch + Duration.hours(10.0) + Duration.seconds(1.0),
        )

        close_approaches = generator.compute_close_approaches(
            trajectory=other_trajectory,
            search_interval=search_interval,
        )

        assert close_approaches is not None
        assert isinstance(close_approaches, list)
        assert len(close_approaches) == 0
