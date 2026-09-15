# Apache License 2.0

import numpy as np
import pytest

from ostk.physics import Environment
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime, Duration, Instant, Scale
from ostk.physics.unit import Length

from ostk.astrodynamics.estimator import ObservationFilter
from ostk.astrodynamics.estimator.observation_filter import MeanElementConsistency
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
    instants = [state.get_instant() + Duration.seconds(60.0 * i) for i in range(30)]
    return propagator.calculate_states_at(state, instants)


@pytest.fixture
def corrupted_observations(observations: list[State]) -> list[State]:
    corrupted = list(observations)
    state = corrupted[10]
    position = state.get_position().get_coordinates()
    unit = position / np.linalg.norm(position)
    corrupted[10] = State(
        state.get_instant(),
        Position.meters(position + 20.0e3 * unit, Frame.GCRF()),
        state.get_velocity(),
    )
    return corrupted


@pytest.fixture
def filter() -> MeanElementConsistency:
    return MeanElementConsistency()


class TestMeanElementConsistency:
    def test_default_constructor(self, filter: MeanElementConsistency):
        assert isinstance(filter, ObservationFilter)
        assert filter.get_gate_factor() == pytest.approx(10.0)
        assert filter.get_minimum_deviation() == Length.kilometers(2.0)

    def test_constructor_with_arguments(self):
        filter = MeanElementConsistency(
            gate_factor=5.0, minimum_deviation=Length.meters(500.0)
        )

        assert filter.get_gate_factor() == pytest.approx(5.0)
        assert filter.get_minimum_deviation() == Length.meters(500.0)

    def test_constructor_invalid_arguments_raise(self):
        with pytest.raises(Exception):
            MeanElementConsistency(gate_factor=0.0)

        with pytest.raises(Exception):
            MeanElementConsistency(minimum_deviation=Length.meters(-1.0))

    def test_channel_names(self):
        assert len(MeanElementConsistency.channel_names()) == 6

    def test_filter_empty(self, filter: MeanElementConsistency):
        analysis = filter.filter([])

        assert isinstance(analysis, MeanElementConsistency.Analysis)
        assert analysis.inlier_mask.size == 0
        assert analysis.deviations.shape == (0, 6)
        assert analysis.gates.shape == (6,)

    def test_filter_clean(
        self, filter: MeanElementConsistency, observations: list[State]
    ):
        analysis = filter.filter(observations)

        assert isinstance(analysis, MeanElementConsistency.Analysis)
        assert isinstance(analysis, ObservationFilter.Analysis)
        assert analysis.inlier_mask.shape == (len(observations),)
        assert analysis.deviations.shape == (len(observations), 6)
        assert analysis.gates.shape == (6,)
        assert analysis.get_inlier_count() == len(observations)
        assert analysis.get_outlier_count() == 0
        assert str(analysis)

    def test_filter_corrupted(
        self, filter: MeanElementConsistency, corrupted_observations: list[State]
    ):
        analysis = filter.filter(corrupted_observations)

        assert analysis.get_outlier_count() == 1
        assert analysis.inlier_mask[10] == 0

    def test_apply(
        self, filter: MeanElementConsistency, corrupted_observations: list[State]
    ):
        inliers = filter.apply(corrupted_observations)

        assert len(inliers) == len(corrupted_observations) - 1
        assert inliers[10] == corrupted_observations[11]

    def test_analysis_constructor(self):
        analysis = MeanElementConsistency.Analysis(
            np.array([1, 0], dtype=int), np.zeros((2, 6)), np.ones(6)
        )

        assert analysis.get_inlier_count() == 1
        assert analysis.deviations.shape == (2, 6)
        assert analysis.gates.shape == (6,)
