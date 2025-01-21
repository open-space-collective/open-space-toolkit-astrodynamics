# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.object import RealInterval

from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics import Environment
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.object import Celestial
from ostk.physics.environment.object.celestial import Earth

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics import Access
from ostk.astrodynamics.access import Generator
from ostk.astrodynamics.access import AccessTarget
from ostk.astrodynamics.access import VisibilityCriterion


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
        access_filter=lambda access: True,
        state_filter=None,
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


@pytest.fixture
def visibility_criterion(environment: Environment) -> VisibilityCriterion:
    return VisibilityCriterion.from_line_of_sight(environment=environment)


@pytest.fixture
def lla() -> LLA:
    return LLA.vector([0.0, 0.0, 500.0])


@pytest.fixture
def access_target(
    visibility_criterion: VisibilityCriterion,
    lla: LLA,
    earth: Earth,
) -> AccessTarget:
    return AccessTarget.from_lla(visibility_criterion, lla, earth)


@pytest.fixture
def trajectory_target(
    visibility_criterion: VisibilityCriterion,
    from_trajectory: Trajectory,
) -> AccessTarget:
    return AccessTarget.from_trajectory(visibility_criterion, from_trajectory)


class TestAccessTarget:
    def test_constructor_success(self, access_target: AccessTarget):
        assert isinstance(access_target, AccessTarget)

    def test_get_type_success(self, access_target: AccessTarget):
        assert access_target.get_type() == AccessTarget.Type.Fixed

    def test_get_visibility_criterion_success(self, access_target: AccessTarget):
        assert access_target.get_visibility_criterion() is not None
        assert isinstance(access_target.get_visibility_criterion(), VisibilityCriterion)

    def test_get_trajectory_success(self, access_target: AccessTarget):
        assert access_target.get_trajectory() is not None
        assert isinstance(access_target.get_trajectory(), Trajectory)

    def test_get_position_success(self, access_target: AccessTarget):
        assert access_target.get_position() is not None
        assert isinstance(access_target.get_position(), Position)

    def test_get_lla_success(
        self,
        access_target: AccessTarget,
        earth: Earth,
    ):
        assert access_target.get_lla(earth) is not None
        assert isinstance(access_target.get_lla(earth), LLA)

    def test_compute_r_sez_ecef_success(
        self,
        access_target: AccessTarget,
        earth: Earth,
    ):
        assert access_target.compute_r_sez_ecef(earth) is not None
        assert isinstance(access_target.compute_r_sez_ecef(earth), np.ndarray)

    def test_from_lla_success(
        self,
        visibility_criterion: VisibilityCriterion,
        lla: LLA,
        earth: Earth,
    ):
        access_target = AccessTarget.from_lla(visibility_criterion, lla, earth)

        assert access_target is not None
        assert isinstance(access_target, AccessTarget)

    def test_from_position_success(
        self,
        visibility_criterion: VisibilityCriterion,
        position: Position,
    ):
        access_target = AccessTarget.from_position(
            visibility_criterion, position.in_frame(Frame.ITRF(), Instant.J2000())
        )

        assert access_target is not None
        assert isinstance(access_target, AccessTarget)

    def test_from_trajectory_success(
        self,
        visibility_criterion: VisibilityCriterion,
        trajectory: Trajectory,
    ):
        access_target = AccessTarget.from_trajectory(visibility_criterion, trajectory)

        assert access_target is not None
        assert isinstance(access_target, AccessTarget)


class TestGenerator:
    def test_constructor_success_environment(self, environment: Environment):
        generator = Generator(
            environment=environment,
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
        assert generator.get_access_filter() is not None
        assert generator.get_state_filter() is None

    def test_get_condition_function_success(
        self,
        generator: Generator,
        trajectory_target: AccessTarget,
        to_trajectory: Trajectory,
    ):
        condition_function = generator.get_condition_function(
            access_target=trajectory_target,
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
        trajectory_target: AccessTarget,
        to_trajectory: Trajectory,
    ):
        accesses = generator.compute_accesses(
            interval=Interval.closed(
                Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
                Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC),
            ),
            access_target=trajectory_target,
            to_trajectory=to_trajectory,
        )

        assert accesses is not None
        assert isinstance(accesses, list)
        assert accesses[0] is not None
        assert isinstance(accesses[0], Access)

    def test_compute_accesses_multiple_targets_success(
        self,
        generator: Generator,
        trajectory_target: AccessTarget,
        to_trajectory: Trajectory,
    ):
        accesses = generator.compute_accesses(
            interval=Interval.closed(
                Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
                Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC),
            ),
            access_targets=[trajectory_target],
            to_trajectory=to_trajectory,
        )

        assert accesses is not None
        assert isinstance(accesses, list)
        assert accesses[0] is not None
        assert isinstance(accesses[0], list)
        assert isinstance(accesses[0][0], Access)

    def test_set_step_success(self, generator: Generator):
        generator.set_step(Duration.seconds(1.0))

    def test_set_tolerance_success(self, generator: Generator):
        generator.set_tolerance(Duration.seconds(1.0))

    def test_set_access_filter_success(self, generator: Generator):
        generator.set_access_filter(access_filter=lambda access: True)

    def test_set_state_filter_success(self, generator: Generator):
        generator.set_state_filter(state_filter=lambda state_1, state_2: True)

    def test_undefined_success(self):
        generator = Generator.undefined()

        assert generator is not None
        assert isinstance(generator, Generator)
        assert generator.is_defined() is False
