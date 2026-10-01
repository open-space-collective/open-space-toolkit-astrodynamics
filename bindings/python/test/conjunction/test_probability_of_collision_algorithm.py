# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale

from ostk.astrodynamics.conjunction import CloseApproach
from ostk.astrodynamics.conjunction import HardBody
from ostk.astrodynamics.conjunction import ProbabilityOfCollisionAlgorithm
from ostk.astrodynamics.conjunction.hard_body import Spherical
from ostk.astrodynamics.estimator import CovarianceMatrix
from ostk.astrodynamics.trajectory import State


class ConstantProbabilityOfCollisionAlgorithm(ProbabilityOfCollisionAlgorithm):
    """Mock algorithm returning a constant probability of collision."""

    def __init__(self, probability_of_collision: float = 0.01):
        super().__init__("Constant")
        self._probability_of_collision: float = probability_of_collision

    def identify_unsatisfied_assumptions(
        self, close_approach: CloseApproach
    ) -> list[str]:
        return []

    def compute_probability_of_collision(
        self,
        close_approach: CloseApproach,
        hard_body_1: HardBody,
        hard_body_2: HardBody,
    ) -> float:
        return self._probability_of_collision


class NotApplicableProbabilityOfCollisionAlgorithm(
    ConstantProbabilityOfCollisionAlgorithm
):
    """Mock algorithm that is never applicable."""

    def identify_unsatisfied_assumptions(
        self, close_approach: CloseApproach
    ) -> list[str]:
        return ["Never applicable"]


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)


def build_state(
    instant: Instant,
    position: list[float],
    velocity: list[float],
    position_sigmas: list[float],
) -> State:
    state: State = State(
        instant,
        Position.meters(position, Frame.GCRF()),
        Velocity.meters_per_second(velocity, Frame.GCRF()),
    )
    state.set_covariance_matrix(
        CovarianceMatrix.from_position_sigmas(instant, position_sigmas, Frame.GCRF())
    )
    return state


@pytest.fixture
def close_approach(instant: Instant) -> CloseApproach:
    return CloseApproach(
        build_state(instant, [7.0e6, 0.0, 0.0], [0.0, 7.5e3, 0.0], [10.0, 100.0, 10.0]),
        build_state(instant, [7.0e6, 0.0, 50.0], [0.0, -7.5e3, 0.0], [20.0, 200.0, 20.0]),
    )


@pytest.fixture
def hard_body() -> Spherical:
    return Spherical(radius=5.0)


@pytest.fixture
def algorithm() -> ConstantProbabilityOfCollisionAlgorithm:
    return ConstantProbabilityOfCollisionAlgorithm()


class TestProbabilityOfCollisionAlgorithm:
    def test_subclass_success(self, algorithm: ConstantProbabilityOfCollisionAlgorithm):
        assert isinstance(algorithm, ProbabilityOfCollisionAlgorithm)

    def test_get_name_success(self, algorithm: ConstantProbabilityOfCollisionAlgorithm):
        assert algorithm.get_name() == "Constant"

    def test_probability_region_success(self):
        assert ProbabilityOfCollisionAlgorithm.ProbabilityRegion.Robust is not None
        assert ProbabilityOfCollisionAlgorithm.ProbabilityRegion.Diluted is not None
        assert ProbabilityOfCollisionAlgorithm.ProbabilityRegion.Maximum is not None

    def test_identify_unsatisfied_assumptions_success(
        self,
        algorithm: ConstantProbabilityOfCollisionAlgorithm,
        close_approach: CloseApproach,
    ):
        unsatisfied_assumptions = algorithm.identify_unsatisfied_assumptions(
            close_approach=close_approach
        )

        assert isinstance(unsatisfied_assumptions, list)

    def test_is_applicable_success(
        self,
        algorithm: ConstantProbabilityOfCollisionAlgorithm,
        close_approach: CloseApproach,
    ):
        assert algorithm.is_applicable(close_approach=close_approach) is True
        assert (
            NotApplicableProbabilityOfCollisionAlgorithm().is_applicable(
                close_approach=close_approach
            )
            is False
        )

    def test_compute_probability_of_collision_success(
        self,
        algorithm: ConstantProbabilityOfCollisionAlgorithm,
        close_approach: CloseApproach,
        hard_body: Spherical,
    ):
        probability_of_collision = algorithm.compute_probability_of_collision(
            close_approach=close_approach,
            hard_body_1=hard_body,
            hard_body_2=hard_body,
        )

        assert 0.0 <= probability_of_collision <= 1.0

    def test_compute_probability_analysis_success(
        self,
        algorithm: ConstantProbabilityOfCollisionAlgorithm,
        close_approach: CloseApproach,
        hard_body: Spherical,
    ):
        analysis = algorithm.compute_probability_analysis(
            close_approach=close_approach,
            hard_body_1=hard_body,
            hard_body_2=hard_body,
        )

        assert isinstance(analysis, ProbabilityOfCollisionAlgorithm.Analysis)
        assert isinstance(
            analysis.region, ProbabilityOfCollisionAlgorithm.ProbabilityRegion
        )
        assert 0.0 <= analysis.probability_of_collision <= 1.0
        assert 0.0 <= analysis.maximum_probability_of_collision <= 1.0

    def test_compute_probability_analysis_success_with_arguments(
        self,
        algorithm: ConstantProbabilityOfCollisionAlgorithm,
        close_approach: CloseApproach,
        hard_body: Spherical,
    ):
        analysis = algorithm.compute_probability_analysis(
            close_approach=close_approach,
            hard_body_1=hard_body,
            hard_body_2=hard_body,
            scale_object_1_covariance=True,
            scale_object_2_covariance=False,
            sigma_scaling_factor_lower_bound=0.1,
            sigma_scaling_factor_upper_bound=10.0,
            maximum_point_per_pass_count=11,
            maximum_pass_count=5,
            convergence_threshold=0.05,
        )

        assert isinstance(analysis, ProbabilityOfCollisionAlgorithm.Analysis)

    def test_compute_probability_of_collision_failure_not_implemented(
        self,
        close_approach: CloseApproach,
        hard_body: Spherical,
    ):
        with pytest.raises(RuntimeError):
            ProbabilityOfCollisionAlgorithm(
                name="Abstract"
            ).compute_probability_of_collision(
                close_approach=close_approach,
                hard_body_1=hard_body,
                hard_body_2=hard_body,
            )
