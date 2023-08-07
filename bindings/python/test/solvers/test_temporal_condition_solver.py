# Apache License 2.0

import pytest

from ostk.physics.units import Length
from ostk.physics.units import Angle
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics import Environment

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.models import Kepler
from ostk.astrodynamics.trajectory.orbit.models.kepler import COE
from ostk.astrodynamics.access import Generator
from ostk.astrodynamics.solvers import TemporalConditionSolver


@pytest.fixture
def temporal_condition_solver() -> TemporalConditionSolver:
    return TemporalConditionSolver(
        time_step=Duration.seconds(30.0),
        tolerance=Duration.milliseconds(1.0),
        maximum_iteration_count=1234,
    )


@pytest.fixture
def interval() -> Interval:
    return Interval.closed(
        Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC),
        Instant.date_time(DateTime(2018, 1, 1, 2, 0, 0), Scale.UTC),
    )


class TestTemporalConditionSolver:
    def test_constructor_success(self):
        temporal_condition_solver = TemporalConditionSolver(
            time_step=Duration.seconds(30.0),
            tolerance=Duration.milliseconds(1.0),
            maximum_iteration_count=1234,
        )

        assert isinstance(temporal_condition_solver, TemporalConditionSolver)

    def test_getters_success(
        self,
        temporal_condition_solver: TemporalConditionSolver,
    ):
        assert temporal_condition_solver.get_time_step() == Duration.seconds(30.0)
        assert temporal_condition_solver.get_tolerance() == Duration.milliseconds(1.0)
        assert temporal_condition_solver.get_maximum_iteration_count() == 1234

    def test_solve_success_one_condition_always_true(
        self,
        temporal_condition_solver: TemporalConditionSolver,
        interval: Interval,
    ):
        solution: list[Interval] = temporal_condition_solver.solve(
            condition=lambda _: True,
            interval=interval,
        )

        assert isinstance(solution, list)
        assert solution == [interval]

    def test_solve_success_one_condition_always_false(
        self,
        temporal_condition_solver: TemporalConditionSolver,
        interval: Interval,
    ):
        solution: list[Interval] = temporal_condition_solver.solve(
            condition=lambda _: False,
            interval=interval,
        )

        assert isinstance(solution, list)
        assert solution == []

    def test_solve_success_multiple_conditions_always_true(
        self,
        temporal_condition_solver: TemporalConditionSolver,
        interval: Interval,
    ):
        solution: list[Interval] = temporal_condition_solver.solve(
            conditions=[
                lambda _: True,
                lambda _: True,
            ],
            interval=interval,
        )

        assert isinstance(solution, list)
        assert solution == [interval]

    def test_solve_success_multiple_conditions_one_always_false(
        self,
        temporal_condition_solver: TemporalConditionSolver,
        interval: Interval,
    ):
        solution: list[Interval] = temporal_condition_solver.solve(
            conditions=[
                lambda _: True,
                lambda _: False,
            ],
            interval=interval,
        )

        assert isinstance(solution, list)
        assert solution == []

    def test_solve_success_using_access_generator(
        self,
        temporal_condition_solver: TemporalConditionSolver,
        interval: Interval,
    ):
        environment = Environment.default()

        generator = Generator(
            environment=environment,
        )

        earth = environment.access_celestial_object_with_name("Earth")

        trajectory = Orbit(
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

        solution: list[Interval] = temporal_condition_solver.solve(
            condition=generator.get_condition_function(
                from_trajectory=trajectory,
                to_trajectory=trajectory,
            ),
            interval=interval,
        )

        assert isinstance(solution, list)
        assert solution == [interval]
