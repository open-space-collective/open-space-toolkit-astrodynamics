# Apache License 2.0

import pytest

import pandas as pd

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth

from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.estimator import ODLeastSquaresSolver
from ostk.astrodynamics.dataframe import generate_states_from_dataframe


@pytest.fixture
def environment() -> Environment:
    return Environment(central_celestial_object=Earth.EGM96(10, 10), objects=[])


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default()


@pytest.fixture
def least_squares_solver() -> LeastSquaresSolver:
    return LeastSquaresSolver.default()


@pytest.fixture
def od_solver(
    environment: Environment,
    numerical_solver: NumericalSolver,
    least_squares_solver: LeastSquaresSolver,
) -> ODLeastSquaresSolver:
    return ODLeastSquaresSolver(
        environment=environment,
        numerical_solver=numerical_solver,
        solver=least_squares_solver,
    )


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [
        CartesianPosition.default(),
        CartesianVelocity.default(),
    ]


@pytest.fixture
def initial_guess_state(
    reference_states: list[State],
) -> State:
    return reference_states[0]


@pytest.fixture
def reference_states() -> list[State]:
    return generate_states_from_dataframe(
        pd.read_csv("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/gnss_data.csv"),
        reference_frame=Frame.ITRF(),
    )


class TestODLeastSquaresSolver:
    def test_constructor(
        self,
        od_solver: ODLeastSquaresSolver,
    ):
        assert isinstance(od_solver, ODLeastSquaresSolver)

    def test_access_methods(
        self,
        od_solver: ODLeastSquaresSolver,
    ):
        assert isinstance(od_solver.access_environment(), Environment)
        assert isinstance(od_solver.access_propagator(), Propagator)
        assert isinstance(od_solver.access_solver(), LeastSquaresSolver)

    def test_estimate_state(
        self,
        od_solver: ODLeastSquaresSolver,
        initial_guess_state: State,
        reference_states: list[State],
        coordinate_subsets: list[CoordinateSubset],
    ):
        analysis = od_solver.estimate_state(
            initial_guess_state=initial_guess_state,
            reference_states=reference_states,
            estimation_coordinate_subsets=coordinate_subsets,
        )

        assert isinstance(analysis, ODLeastSquaresSolver.Analysis)
        assert isinstance(analysis.get_determined_state(), State)
        assert isinstance(analysis.get_solver_analysis(), LeastSquaresSolver.Analysis)

    def test_estimate_orbit(
        self,
        od_solver: ODLeastSquaresSolver,
        initial_guess_state: State,
        reference_states: list[State],
        coordinate_subsets: list[CoordinateSubset],
    ):
        orbit = od_solver.estimate_orbit(
            initial_guess_state=initial_guess_state,
            reference_states=reference_states,
            estimation_coordinate_subsets=coordinate_subsets,
        )

        assert isinstance(orbit, Orbit)
