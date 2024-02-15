# Apache License 2.0

import pytest
import math

import numpy as np

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant
from ostk.physics.time import Duration

from ostk.astrodynamics.solver import FiniteDifferenceSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateSubset


@pytest.fixture
def step_percentage() -> float:
    return 1e-3


@pytest.fixture
def step_duration() -> Duration:
    return Duration.seconds(1e-6)


@pytest.fixture(
    params=[
        FiniteDifferenceSolver.Type.Forward,
        FiniteDifferenceSolver.Type.Backward,
        FiniteDifferenceSolver.Type.Central,
    ]
)
def finite_difference_solver(request, step_percentage: float, step_duration: Duration):
    return FiniteDifferenceSolver(request.param, step_percentage, step_duration)


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [CoordinateSubset("Position", 1), CoordinateSubset("Velocity", 1)]


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def state(
    initial_instant: Instant, frame: Frame, coordinate_subsets: list[CoordinateSubset]
):
    return State(initial_instant, [1, 0], frame, coordinate_subsets)


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
def generate_states_coordinates() -> callable:
    def state_fn(state, instants) -> np.ndarray:
        x0: float = state.get_coordinates()[0]
        v0: float = state.get_coordinates()[1]
        omega: float = 1.0

        states_coordinates: list[list[float]] = []

        for instant in instants:
            t: float = (instant - state.get_instant()).in_seconds()
            x: float = x0 * math.cos(omega * t) + v0 / omega * math.sin(omega * t)
            v: float = -x0 * omega * math.sin(omega * t) + v0 * math.cos(omega * t)

            coordinates: list[float] = [x, v]

            states_coordinates.append(coordinates)

        return np.array(states_coordinates)

    return state_fn


@pytest.fixture
def generate_state_coordinates() -> callable:
    def state_fn(state, instant) -> np.ndarray:
        x0: float = state.get_coordinates()[0]
        v0: float = state.get_coordinates()[1]
        omega: float = 1.0

        t: float = (instant - state.get_instant()).in_seconds()
        x: float = x0 * math.cos(omega * t) + v0 / omega * math.sin(omega * t)
        v: float = -x0 * omega * math.sin(omega * t) + v0 * math.cos(omega * t)

        return np.array([x, v])

    return state_fn


class TestFiniteDifferenceSolver:
    def test_constructor(self, finite_difference_solver: FiniteDifferenceSolver):
        assert isinstance(finite_difference_solver, FiniteDifferenceSolver)

    def test_getters(self, finite_difference_solver: FiniteDifferenceSolver):
        assert isinstance(
            finite_difference_solver.get_type(), FiniteDifferenceSolver.Type
        )
        assert finite_difference_solver.get_step_percentage() is not None
        assert isinstance(finite_difference_solver.get_step_duration(), Duration)

    def test_string_from_type(self):
        assert (
            FiniteDifferenceSolver.string_from_type(FiniteDifferenceSolver.Type.Forward)
            == "Forward"
        )

    def test_compute_jacobian_array(
        self,
        finite_difference_solver: FiniteDifferenceSolver,
        state: State,
        instants: list[Instant],
        generate_states_coordinates: callable,
    ):
        stm = finite_difference_solver.compute_jacobian(
            state=state,
            instants=instants,
            generate_states_coordinates=generate_states_coordinates,
            coordinates_dimension=2,
        )
        assert isinstance(stm, np.ndarray)
        assert stm.shape == (
            len(state.get_coordinates()) * len(instants),
            len(state.get_coordinates()),
        )

    def test_compute_jacobian_single(
        self,
        finite_difference_solver: FiniteDifferenceSolver,
        state: State,
        generate_state_coordinates: callable,
        instant: Instant,
    ):
        stm = finite_difference_solver.compute_jacobian(
            state=state,
            instant=instant,
            generate_state_coordinates=generate_state_coordinates,
            coordinates_dimension=2,
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
        generate_state_coordinates: callable,
    ):
        gradient = finite_difference_solver.compute_gradient(
            state=state,
            generate_state_coordinates=generate_state_coordinates,
        )
        assert isinstance(gradient, np.ndarray)
        assert all(gradient - np.array([0.0, -1.0]) < 1e-6)

    def test_default(self):
        assert isinstance(FiniteDifferenceSolver.default(), FiniteDifferenceSolver)
