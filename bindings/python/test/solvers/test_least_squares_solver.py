# Apache License 2.0

import pytest
import numpy as np

from ostk.physics.time import Instant, Duration
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateSubset


@pytest.fixture
def max_iteration_count() -> int:
    return 20


@pytest.fixture
def rms_update_threshold() -> float:
    return 1.0


@pytest.fixture
def least_squares_solver(
    max_iteration_count: int, rms_update_threshold: float
) -> LeastSquaresSolver:
    return LeastSquaresSolver(max_iteration_count, rms_update_threshold)


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [CoordinateSubset("Position", 1), CoordinateSubset("Velocity", 1)]


@pytest.fixture
def initial_instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def initial_guess_state(
    initial_instant: Instant,
    coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
) -> State:
    return State(initial_instant, [1.0, 0.0], frame, coordinate_subsets)


@pytest.fixture
def reference_states(
    initial_instant: Instant,
    coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
) -> list[State]:
    return [
        State(
            initial_instant + Duration.seconds(float(x)),
            [np.cos(x), -np.sin(x)],
            frame,
            coordinate_subsets,
        )
        for x in range(0, 200, 10)
    ]


@pytest.fixture
def generate_states_callback() -> callable:
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


class TestLeastSquaresSolver:
    def test_constructor(
        self,
        least_squares_solver: LeastSquaresSolver,
    ):
        assert isinstance(least_squares_solver, LeastSquaresSolver)

    def test_getters(
        self,
        least_squares_solver: LeastSquaresSolver,
    ):
        assert least_squares_solver.get_max_iteration_count() == 20
        assert least_squares_solver.get_rms_update_threshold() == 1.0

    def test_solve(
        self,
        least_squares_solver: LeastSquaresSolver,
        initial_guess_state: State,
        reference_states: list[State],
        generate_states_callback: callable,
    ):
        analysis = least_squares_solver.solve(
            initial_guess_state=initial_guess_state,
            reference_states=reference_states,
            generate_states_callback=generate_states_callback,
        )
        assert isinstance(analysis, LeastSquaresSolver.Analysis)
        assert analysis.get_rms_error() is not None
        assert analysis.get_iteration_count() is not None
        assert analysis.get_termination_criteria() is not None
        assert isinstance(analysis.access_solution_state(), State)
        assert isinstance(analysis.access_solution_covariance(), np.ndarray)
        assert isinstance(analysis.access_solution_frisbee_covariance(), np.ndarray)
        assert isinstance(analysis.access_steps(), list)

    def test_calculate_empirical_covariance(
        self,
        reference_states: list[State],
    ):
        covariance: np.ndarray = LeastSquaresSolver.calculate_empirical_covariance(
            reference_states
        )

        assert isinstance(covariance, np.ndarray)
        assert covariance.shape == (2, 2)

    def test_default(self):
        default_solver: LeastSquaresSolver = LeastSquaresSolver.default()

        assert isinstance(default_solver, LeastSquaresSolver)
        assert default_solver.get_max_iteration_count() == 20
        assert default_solver.get_rms_update_threshold() == 1.0
