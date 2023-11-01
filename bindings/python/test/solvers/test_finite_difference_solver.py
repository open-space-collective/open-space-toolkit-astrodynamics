# Apache License 2.0

import pytest
import math

import numpy as np

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant
from ostk.physics.time import Duration

from ostk.astrodynamics.solvers import FiniteDifferenceSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinatesSubset


@pytest.fixture(
    params=[
        FiniteDifferenceSolver.Type.Forward,
        FiniteDifferenceSolver.Type.Backward,
        FiniteDifferenceSolver.Type.Central,
    ]
)
def finite_difference_solver(request):
    return FiniteDifferenceSolver(request.param)


@pytest.fixture
def coordinates_subsets() -> list[CoordinatesSubset]:
    return [CoordinatesSubset("Position", 1), CoordinatesSubset("Velocity", 1)]


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def state(
    initial_instant: Instant, frame: Frame, coordinates_subsets: list[CoordinatesSubset]
):
    return State(initial_instant, [1, 0], frame, coordinates_subsets)


@pytest.fixture
def initial_instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def instant(initial_instant: Instant) -> Instant:
    return initial_instant + Duration.seconds(100.0)


@pytest.fixture
def instants(initial_instant: Instant) -> list[Instant]:
    return [
        initial_instant + Duration.seconds(100.0),
        initial_instant + Duration.seconds(200.0),
    ]


@pytest.fixture
def get_states() -> callable:
    def state_fn(state, instants) -> list[State]:
        x0: float = state.get_coordinates()[0]
        v0: float = state.get_coordinates()[1]
        omega: float = 1.0

        states: list[State] = []

        for instant in instants:
            t: float = (instant - state.get_instant()).in_seconds()
            x: float = x0 * math.cos(omega * t) + v0 / omega * math.sin(omega * t)
            v: float = -x0 * omega * math.sin(omega * t) + v0 * math.cos(omega * t)

            coordinates: list[float] = [x, v]

            states.append(
                State(
                    instant,
                    coordinates,
                    state.get_frame(),
                    state.get_coordinates_subsets(),
                )
            )

        return states

    return state_fn


@pytest.fixture
def get_state() -> callable:
    def state_fn(state, instant) -> list[State]:
        x0: float = state.get_coordinates()[0]
        v0: float = state.get_coordinates()[1]
        omega: float = 1.0

        t: float = (instant - state.get_instant()).in_seconds()
        x: float = x0 * math.cos(omega * t) + v0 / omega * math.sin(omega * t)
        v: float = -x0 * omega * math.sin(omega * t) + v0 * math.cos(omega * t)

        coordinates: list[float] = [x, v]

        return State(
            instant,
            coordinates,
            state.get_frame(),
            state.get_coordinates_subsets(),
        )

    return state_fn


class TestFiniteDifferenceSolver:
    def test_constructor(self, finite_difference_solver: FiniteDifferenceSolver):
        assert isinstance(finite_difference_solver, FiniteDifferenceSolver)

    def test_string_from_type(self):
        assert (
            FiniteDifferenceSolver.string_from_type(FiniteDifferenceSolver.Type.Forward)
            == "Forward"
        )

    def test_compute_state_transition_matrix_array(
        self,
        finite_difference_solver: FiniteDifferenceSolver,
        state: State,
        instants: list[Instant],
        get_states,
    ):
        stm = finite_difference_solver.compute_state_transition_matrix(
            state, instants, get_states, 1e-3
        )
        assert isinstance(stm, np.ndarray)
        assert stm.shape == (
            len(state.get_coordinates()),
            len(state.get_coordinates()) * len(instants),
        )

    def test_compute_state_transition_matrix_single(
        self,
        finite_difference_solver: FiniteDifferenceSolver,
        state: State,
        get_state: callable,
        instant: Instant,
    ):
        stm = finite_difference_solver.compute_state_transition_matrix(
            state, instant, get_state, 1e-3
        )
        assert isinstance(stm, np.ndarray)
        assert stm.shape == (
            len(state.get_coordinates()),
            len(state.get_coordinates()),
        )

    def test_compute_gradient(
        self,
        finite_difference_solver: FiniteDifferenceSolver,
        state: State,
        get_state: callable,
    ):
        gradient = finite_difference_solver.compute_gradient(
            state, get_state, Duration.milliseconds(1e-3)
        )
        assert isinstance(gradient, np.ndarray)
        assert all(gradient - np.array([0.0, -1.0]) < 1e-6)
