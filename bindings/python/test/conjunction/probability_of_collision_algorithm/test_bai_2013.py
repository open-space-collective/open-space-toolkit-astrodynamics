# Apache License 2.0

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.environment.gravitational import Earth
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.unit import Derived

from ostk.astrodynamics.conjunction import CloseApproach
from ostk.astrodynamics.conjunction import ProbabilityOfCollisionAlgorithm
from ostk.astrodynamics.conjunction.hard_body import Spherical
from ostk.astrodynamics.conjunction.probability_of_collision_algorithm import Bai2013
from ostk.astrodynamics.estimator import CovarianceMatrix
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def object_1_state(instant: Instant) -> State:
    state: State = State(
        instant,
        Position.meters([-1457273.246, 1589568.484, 6814189.959], Frame.GCRF()),
        Velocity.meters_per_second([-7001.731, -2439.512, -926.209], Frame.GCRF()),
    )
    state.set_covariance_matrix(
        CovarianceMatrix.from_position_sigmas(
            instant, [23.1207, 206.1885, 71.9775], Frame.GCRF()
        )
    )
    return state


@pytest.fixture
def object_2_state(instant: Instant) -> State:
    state: State = State(
        instant,
        Position.meters([-1457532.155, 1588932.671, 6814316.188], Frame.GCRF()),
        Velocity.meters_per_second([3578.705, -6172.896, 2200.215], Frame.GCRF()),
    )
    state.set_covariance_matrix(
        CovarianceMatrix.from_position_sigmas(
            instant, [36.3234, 410.2069, 34.1134], Frame.GCRF()
        )
    )
    return state


@pytest.fixture
def close_approach(object_1_state: State, object_2_state: State) -> CloseApproach:
    return CloseApproach(object_1_state, object_2_state)


@pytest.fixture
def hard_body() -> Spherical:
    return Spherical(radius=5.0)


@pytest.fixture
def algorithm() -> Bai2013:
    return Bai2013()


class TestBai2013:
    def test_constructor_success(self, algorithm: Bai2013):
        assert isinstance(algorithm, Bai2013)
        assert isinstance(algorithm, ProbabilityOfCollisionAlgorithm)

        assert isinstance(
            Bai2013(
                relative_velocity_threshold=Derived(
                    100.0, Derived.Unit.meter_per_second()
                ),
                eccentricity_threshold=0.001,
                frame=Frame.GCRF(),
                gravitational_parameter=Earth.EGM96.gravitational_parameter,
            ),
            Bai2013,
        )

    def test_get_name_success(self, algorithm: Bai2013):
        assert algorithm.get_name() == "Bai 2013"

    def test_identify_unsatisfied_assumptions_success(
        self, algorithm: Bai2013, close_approach: CloseApproach
    ):
        unsatisfied_assumptions = algorithm.identify_unsatisfied_assumptions(
            close_approach=close_approach
        )

        assert isinstance(unsatisfied_assumptions, list)
        assert all(isinstance(assumption, str) for assumption in unsatisfied_assumptions)

    @pytest.mark.parametrize(
        "eccentricity_threshold",
        [1.0, 0.0],
        ids=["explicit", "modified"],
    )
    def test_compute_probability_of_collision_success(
        self,
        close_approach: CloseApproach,
        hard_body: Spherical,
        eccentricity_threshold: float,
    ):
        algorithm: Bai2013 = Bai2013(eccentricity_threshold=eccentricity_threshold)

        probability_of_collision = algorithm.compute_probability_of_collision(
            close_approach=close_approach,
            hard_body_1=hard_body,
            hard_body_2=hard_body,
        )

        assert 0.0 <= probability_of_collision <= 1.0
