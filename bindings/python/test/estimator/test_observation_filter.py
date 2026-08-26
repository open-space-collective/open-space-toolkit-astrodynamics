# Apache License 2.0

import numpy as np
import pytest

from ostk.physics import Environment
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime, Duration, Instant, Scale

from ostk.astrodynamics.estimator import MeanElementConsistency, ObservationFilter
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

    def test_mean_element_consistency_is_observation_filter(self):
        f = MeanElementConsistency()
        assert isinstance(f, ObservationFilter)


class TestMeanElementConsistency:
    def test_construct_with_defaults(self):
        f = MeanElementConsistency()
        assert f is not None

    def test_construct_with_all_args(self):
        f = MeanElementConsistency(
            gate_factor=15.0,
            noise_floor=5.0,
        )
        assert f.get_gate_factor() == pytest.approx(15.0)
        assert f.get_noise_floor() == pytest.approx(5.0)
        assert f.get_gravitational_parameter() is not None

    def test_invalid_gate_factor_raises(self):
        with pytest.raises(Exception):
            MeanElementConsistency(gate_factor=0.0)

    def test_invalid_noise_floor_raises(self):
        with pytest.raises(Exception):
            MeanElementConsistency(noise_floor=-1.0)

    def test_filter_empty(self):
        f = MeanElementConsistency()
        analysis = f.filter([])
        assert isinstance(analysis, MeanElementConsistency.Analysis)
        assert analysis.inlier_mask.size == 0
        assert analysis.get_inlier_count() == 0
        assert analysis.get_outlier_count() == 0
        assert analysis.get_filtered_observations([]) == []
        assert analysis.channel_residuals.shape[0] == 0

    def test_filter_keeps_good_observations(self, observations: list[State]):
        f = MeanElementConsistency()
        analysis = f.filter(observations)
        assert isinstance(analysis, MeanElementConsistency.Analysis)
        assert analysis.inlier_mask.size == len(observations)
        assert np.all(analysis.inlier_mask == 1)
        assert analysis.get_inlier_count() == len(observations)
        assert analysis.get_outlier_count() == 0
        assert len(analysis.get_filtered_observations(observations)) == len(observations)
        assert analysis.channel_residuals.shape == (len(observations), 6)
        assert analysis.applied_channel_gates.size == 6
        assert np.all(analysis.applied_channel_gates > 0.0)

    def test_filter_rejects_glitched_observation(self, observations: list[State]):
        glitched = list(observations)
        middle = len(glitched) // 2
        along_track = np.array(
            glitched[middle].get_velocity().get_coordinates(), dtype=float
        )
        along_track /= np.linalg.norm(along_track)
        glitched[middle] = State(
            glitched[middle].get_instant(),
            Position.meters(
                np.array(glitched[middle].get_position().get_coordinates(), dtype=float)
                + 5000.0 * along_track,
                Frame.GCRF(),
            ),
            glitched[middle].get_velocity(),
        )

        f = MeanElementConsistency()
        analysis = f.filter(glitched)

        assert analysis.get_outlier_count() == 1
        assert analysis.inlier_mask[middle] == 0

    def test_analysis_str(self, observations: list[State]):
        f = MeanElementConsistency()
        analysis = f.filter(observations)
        assert str(analysis)
        assert repr(analysis)
