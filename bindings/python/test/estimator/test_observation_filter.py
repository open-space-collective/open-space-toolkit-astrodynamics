# Apache License 2.0

import numpy as np
import pytest

from ostk.physics import Environment
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime, Duration, Instant, Scale

from ostk.astrodynamics.estimator import ObservationFilter, RANSAC
from ostk.astrodynamics.trajectory import Propagator, State
from ostk.astrodynamics.trajectory.state import NumericalSolver


@pytest.fixture
def environment() -> Environment:
    return Environment(Earth.EGM96(10, 10), [])


@pytest.fixture
def observations(environment: Environment) -> list[State]:
    state = State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )
    propagator = Propagator.from_environment(NumericalSolver.default(), environment)
    instants = [state.get_instant() + Duration.seconds(float(i * 60)) for i in range(20)]
    return propagator.calculate_states_at(state, instants)


class TestObservationFilter:
    def test_python_subclass(self, observations: list[State]):
        class KeepFirstHalf(ObservationFilter):
            def filter(self, obs):
                half_count = len(obs) // 2
                mask = np.zeros(len(obs), dtype=np.int32)
                mask[:half_count] = 1
                return ObservationFilter.Analysis(mask)

        f = KeepFirstHalf()
        analysis = f.filter(observations)
        assert isinstance(analysis, ObservationFilter.Analysis)
        assert analysis.get_inlier_count() == len(observations) // 2
        assert analysis.get_outlier_count() == len(observations) - len(observations) // 2
        filtered = analysis.get_filtered_observations(observations)
        assert len(filtered) == len(observations) // 2

    def test_ransac_is_observation_filter(self, environment: Environment):
        f = RANSAC(environment)
        assert isinstance(f, ObservationFilter)


class TestRANSAC:
    def test_construct_from_environment(self, environment: Environment):
        f = RANSAC(environment)
        assert f is not None

    def test_construct_from_propagator(self, environment: Environment):
        propagator = Propagator.from_environment(NumericalSolver.default(), environment)
        f = RANSAC(propagator)
        assert f is not None

    def test_construct_with_all_args(self, environment: Environment):
        f = RANSAC(
            environment=environment,
            numerical_solver=NumericalSolver.default(),
            inlier_threshold=5.0e3,
            max_iteration_count=20,
            min_inlier_ratio=0.9,
            random_seed=7,
        )
        assert f.get_inlier_threshold() == pytest.approx(5.0e3)
        assert f.get_max_iteration_count() == 20
        assert f.get_min_inlier_ratio() == pytest.approx(0.9)
        assert f.get_random_seed() == 7
        assert f.access_propagator() is not None

    def test_invalid_threshold_raises(self, environment: Environment):
        with pytest.raises(Exception):
            RANSAC(environment, inlier_threshold=-1.0)

    def test_invalid_max_iterations_raises(self, environment: Environment):
        with pytest.raises(Exception):
            RANSAC(environment, max_iteration_count=0)

    def test_filter_empty(self, environment: Environment):
        f = RANSAC(environment)
        analysis = f.filter([])
        assert isinstance(analysis, RANSAC.Analysis)
        assert analysis.inlier_mask.size == 0
        assert analysis.get_inlier_count() == 0
        assert analysis.get_outlier_count() == 0
        assert analysis.get_filtered_observations([]) == []
        assert analysis.iteration_count == 0
        assert analysis.early_termination is False
        assert analysis.best_inlier_count == 0
        assert analysis.seed_failure_count == 0

    def test_filter_keeps_good_observations(
        self, environment: Environment, observations: list[State]
    ):
        f = RANSAC(
            environment=environment,
            inlier_threshold=1.0,
            max_iteration_count=5,
            min_inlier_ratio=0.99,
            random_seed=1,
        )
        analysis = f.filter(observations)
        assert isinstance(analysis, RANSAC.Analysis)
        assert analysis.inlier_mask.size == len(observations)
        assert np.all(analysis.inlier_mask == 1)
        assert analysis.get_inlier_count() == len(observations)
        assert analysis.get_outlier_count() == 0
        filtered = analysis.get_filtered_observations(observations)
        assert len(filtered) == len(observations)
        assert analysis.best_inlier_count == len(observations)
        # All observations match → very first iteration should hit ratio target
        assert analysis.early_termination is True
        assert analysis.iteration_count == 1

    def test_analysis_str(self, environment: Environment, observations: list[State]):
        f = RANSAC(
            environment,
            inlier_threshold=1.0,
            max_iteration_count=1,
            min_inlier_ratio=0.0,
            random_seed=1,
        )
        analysis = f.filter(observations)
        assert str(analysis)
