# Apache License 2.0

import numpy as np
import pytest

from ostk.physics import Environment
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime, Duration, Instant, Scale

from ostk.astrodynamics.estimator import ObservationFilter
from ostk.astrodynamics.trajectory import Propagator, State
from ostk.astrodynamics.trajectory.state import NumericalSolver


@pytest.fixture
def observations() -> list[State]:
    state = State(
        Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC),
        Position.meters([6878.0e3, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 1000.0, 7500.0], Frame.GCRF()),
    )
    propagator = Propagator.from_environment(
        NumericalSolver.default(), Environment(Earth.EGM2008(2, 0), [])
    )
    instants = [state.get_instant() + Duration.seconds(60.0 * i) for i in range(20)]
    return propagator.calculate_states_at(state, instants)


class KeepFirstHalf(ObservationFilter):
    def filter(self, observations: list[State]) -> ObservationFilter.Analysis:
        mask = np.zeros(len(observations), dtype=int)
        mask[: len(observations) // 2] = 1
        return ObservationFilter.Analysis(mask)


class TestObservationFilterAnalysis:
    def test_constructor_and_counts(self):
        analysis = ObservationFilter.Analysis(np.array([1, 0, 1, 1], dtype=int))

        assert analysis.get_inlier_count() == 3
        assert analysis.get_outlier_count() == 1
        assert list(analysis.inlier_mask) == [1, 0, 1, 1]
        assert str(analysis)

    def test_get_filtered_observations(self, observations: list[State]):
        mask = np.ones(len(observations), dtype=int)
        mask[3] = 0
        analysis = ObservationFilter.Analysis(mask)

        filtered = analysis.get_filtered_observations(observations)

        assert len(filtered) == len(observations) - 1
        assert filtered[3] == observations[4]

    def test_get_filtered_observations_size_mismatch_raises(
        self, observations: list[State]
    ):
        analysis = ObservationFilter.Analysis(np.ones(3, dtype=int))

        with pytest.raises(Exception):
            analysis.get_filtered_observations(observations)


class TestObservationFilter:
    def test_python_subclass_filter(self, observations: list[State]):
        analysis = KeepFirstHalf().filter(observations)

        assert isinstance(analysis, ObservationFilter.Analysis)
        assert analysis.get_inlier_count() == len(observations) // 2
        assert analysis.get_outlier_count() == len(observations) - len(observations) // 2

    def test_python_subclass_apply(self, observations: list[State]):
        inliers = KeepFirstHalf().apply(observations)

        assert len(inliers) == len(observations) // 2
        assert inliers[0] == observations[0]
