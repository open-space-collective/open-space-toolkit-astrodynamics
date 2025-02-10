# Apache License 2.0

from typing import Callable

import pytest
import numpy as np

from ostk.core.type import Real
from ostk.core.type import String

from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.solver import FiniteDifferenceSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateSubset


@pytest.fixture
def rms_error() -> float:
    return 1.0


@pytest.fixture
def x_hat() -> np.ndarray:
    return np.array([1.0, 0.0])


@pytest.fixture
def step(
    rms_error: float,
    x_hat: np.ndarray,
) -> LeastSquaresSolver.Step:
    return LeastSquaresSolver.Step(
        rms_error=rms_error,
        x_hat=x_hat,
    )


@pytest.fixture
def termination_criteria() -> str:
    return "RMS Update Threshold"


@pytest.fixture
def estimated_state(coordinate_subsets: list[CoordinateSubset]) -> State:
    return State(
        Instant.J2000(),
        [1.0, 0.0],
        Frame.GCRF(),
        coordinate_subsets,
    )


@pytest.fixture
def estimated_covariance() -> np.ndarray:
    return np.array([[1.0, 0.0], [0.0, 1.0]])


@pytest.fixture
def estimated_frisbee_covariance() -> np.ndarray:
    return np.array([[1.0, 0.0], [0.0, 1.0]])


@pytest.fixture
def observation_count() -> int:
    return 10


@pytest.fixture
def computed_observations(
    observations: list[State],
) -> list[State]:
    return observations


@pytest.fixture
def steps(step: LeastSquaresSolver.Step) -> list[LeastSquaresSolver.Step]:
    return [step]


@pytest.fixture
def analysis(
    termination_criteria: str,
    estimated_state: State,
    estimated_covariance: np.ndarray,
    estimated_frisbee_covariance: np.ndarray,
    computed_observations: list[State],
    steps: list[LeastSquaresSolver.Step],
) -> LeastSquaresSolver.Analysis:
    return LeastSquaresSolver.Analysis(
        termination_criteria=termination_criteria,
        estimated_state=estimated_state,
        estimated_covariance=estimated_covariance,
        estimated_frisbee_covariance=estimated_frisbee_covariance,
        computed_observations=computed_observations,
        steps=steps,
    )


@pytest.fixture
def max_iteration_count() -> int:
    return 20


@pytest.fixture
def rms_update_threshold() -> float:
    return 1.0


@pytest.fixture
def finite_difference_solver() -> FiniteDifferenceSolver:
    return FiniteDifferenceSolver.default()


@pytest.fixture
def least_squares_solver(
    max_iteration_count: int,
    rms_update_threshold: float,
    finite_difference_solver: FiniteDifferenceSolver,
) -> LeastSquaresSolver:
    return LeastSquaresSolver(
        maximum_iteration_count=max_iteration_count,
        rms_update_threshold=rms_update_threshold,
        finite_difference_solver=finite_difference_solver,
    )


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [CoordinateSubset("Position", 1), CoordinateSubset("Velocity", 1)]


@pytest.fixture
def initial_guess_sigmas(
    coordinate_subsets: list[CoordinateSubset],
) -> dict[CoordinateSubset, list[float]]:
    return {
        coordinate_subsets[0]: [1e-1],
        coordinate_subsets[1]: [1e-2],
    }


@pytest.fixture
def observation_sigmas(
    coordinate_subsets: list[CoordinateSubset],
) -> dict[CoordinateSubset, list[float]]:
    return {
        coordinate_subsets[0]: [1e-1],
        coordinate_subsets[1]: [1e-2],
    }


@pytest.fixture
def initial_instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def initial_guess(
    initial_instant: Instant,
    coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
) -> State:
    return State(initial_instant, [1.0, 0.0], frame, coordinate_subsets)


@pytest.fixture
def observations(
    initial_instant: Instant,
    coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
    observation_count: int,
) -> list[State]:
    return [
        State(
            initial_instant + Duration.seconds(float(x)),
            [np.cos(x), -np.sin(x)],
            frame,
            coordinate_subsets,
        )
        for x in range(0, observation_count)
    ]


@pytest.fixture
def state_generator() -> Callable:
    def state_fn(state, instants) -> list[State]:
        x0: float = state.get_coordinates()[0]
        v0: float = state.get_coordinates()[1]
        omega: float = 1.0

        states: list[State] = []

        for instant in instants:
            t: float = float((instant - state.get_instant()).in_seconds())
            x: float = x0 * np.cos(omega * t) + v0 / omega * np.sin(omega * t)
            v: float = -x0 * omega * np.sin(omega * t) + v0 * np.cos(omega * t)

            states.append(
                State(instant, [x, v], Frame.GCRF(), state.get_coordinate_subsets())
            )

        return states

    return state_fn


class TestLeastSquaresSolverStep:

    def test_constructor(
        self,
        step: LeastSquaresSolver.Step,
    ):
        assert isinstance(step, LeastSquaresSolver.Step)

    def test_getters(
        self,
        step: LeastSquaresSolver.Step,
        rms_error: float,
        x_hat: np.ndarray,
    ):
        assert step.rms_error == rms_error
        assert np.array_equal(step.x_hat, x_hat)


class TestLeastSquaresSolverAnalysis:

    def test_constructor(
        self,
        analysis: LeastSquaresSolver.Analysis,
    ):
        assert isinstance(analysis, LeastSquaresSolver.Analysis)

    def test_getters(
        self,
        analysis: LeastSquaresSolver.Analysis,
    ):
        assert isinstance(analysis.rms_error, Real)
        assert isinstance(analysis.observation_count, int)
        assert isinstance(analysis.iteration_count, int)
        assert isinstance(analysis.termination_criteria, String)
        assert isinstance(analysis.estimated_state, State)
        assert isinstance(analysis.estimated_covariance, np.ndarray)
        assert isinstance(analysis.estimated_frisbee_covariance, np.ndarray)
        assert isinstance(analysis.computed_observations, list)
        assert isinstance(analysis.steps, list)

    def test_compute_residual_states(
        self,
        analysis: LeastSquaresSolver.Analysis,
        observations: list[State],
    ):
        residuals: list[State] = analysis.compute_residual_states(
            observations=observations
        )

        assert isinstance(residuals, list)
        assert len(residuals) == len(observations)


class TestLeastSquaresSolver:
    def test_constructor(
        self,
        least_squares_solver: LeastSquaresSolver,
    ):
        assert isinstance(least_squares_solver, LeastSquaresSolver)

    def test_getters(
        self,
        least_squares_solver: LeastSquaresSolver,
        max_iteration_count: int,
        rms_update_threshold: float,
    ):
        assert least_squares_solver.get_max_iteration_count() == max_iteration_count
        assert least_squares_solver.get_rms_update_threshold() == rms_update_threshold
        assert least_squares_solver.get_finite_difference_solver() is not None

    def test_solve_defaults(
        self,
        least_squares_solver: LeastSquaresSolver,
        initial_guess: State,
        observations: list[State],
        state_generator: callable,
    ):
        analysis = least_squares_solver.solve(
            initial_guess=initial_guess,
            observations=observations,
            state_generator=state_generator,
        )

        assert analysis is not None

    def test_solve(
        self,
        least_squares_solver: LeastSquaresSolver,
        initial_guess: State,
        observations: list[State],
        state_generator: callable,
        initial_guess_sigmas: dict[CoordinateSubset, list[float]],
        observation_sigmas: dict[CoordinateSubset, list[float]],
    ):
        analysis = least_squares_solver.solve(
            initial_guess=initial_guess,
            observations=observations,
            state_generator=state_generator,
            initial_guess_sigmas=initial_guess_sigmas,
            observation_sigmas=observation_sigmas,
        )

        assert analysis is not None

    def test_calculate_empirical_covariance(
        self,
        observations: list[State],
    ):
        covariance: np.ndarray = LeastSquaresSolver.calculate_empirical_covariance(
            observations
        )

        assert isinstance(covariance, np.ndarray)
        assert covariance.shape == (2, 2)

    def test_default(self):
        default_solver: LeastSquaresSolver = LeastSquaresSolver.default()

        assert isinstance(default_solver, LeastSquaresSolver)
