# Apache License 2.0

import pytest

import pandas as pd

import numpy as np

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
from ostk.astrodynamics.estimator import OrbitDeterminationSolver
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
def orbit_determination_solver(
    environment: Environment,
    numerical_solver: NumericalSolver,
    least_squares_solver: LeastSquaresSolver,
) -> OrbitDeterminationSolver:
    return OrbitDeterminationSolver(
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
def initial_guess(
    observations: list[State],
) -> State:
    return observations[0]


@pytest.fixture
def observations() -> list[State]:
    return generate_states_from_dataframe(
        pd.read_csv(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolverData/gnss_data.csv"
        ),
        reference_frame=Frame.ITRF(),
    )


@pytest.fixture
def initial_guess_sigmas(
    coordinate_subsets: list[CoordinateSubset],
) -> dict[CoordinateSubset, list[float]]:
    return {
        coordinate_subsets[0]: [1e-1, 1e-1, 1e-1],
        coordinate_subsets[1]: [1e-2, 1e-2, 1e-2],
    }


@pytest.fixture
def observation_sigmas(
    coordinate_subsets: list[CoordinateSubset],
) -> dict[CoordinateSubset, list[float]]:
    return {
        coordinate_subsets[0]: [1e-1, 1e-1, 1e-1],
        coordinate_subsets[1]: [1e-2, 1e-2, 1e-2],
    }


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
def observation_count() -> int:
    return 5


@pytest.fixture
def termination_criteria() -> str:
    return "RMS Update Threshold"


@pytest.fixture
def estimated_state() -> State:
    return State(
        Instant.J2000(),
        [1.0, 0.0],
        Frame.GCRF(),
        [CoordinateSubset("Position", 1), CoordinateSubset("Velocity", 1)],
    )


@pytest.fixture
def estimated_covariance() -> np.ndarray:
    return np.array([[1.0, 0.0], [0.0, 1.0]])


@pytest.fixture
def estimated_frisbee_covariance() -> np.ndarray:
    return np.array([[1.0, 0.0], [0.0, 1.0]])


@pytest.fixture
def computed_observations(
    observations: list[State],
) -> list[State]:
    return observations


@pytest.fixture
def steps(step: LeastSquaresSolver.Step) -> list[LeastSquaresSolver.Step]:
    return [step]


@pytest.fixture
def solver_analysis(
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
def analysis(
    initial_guess: State,
    solver_analysis: LeastSquaresSolver.Analysis,
) -> OrbitDeterminationSolver.Analysis:
    return OrbitDeterminationSolver.Analysis(
        estimated_state=initial_guess,
        solver_analysis=solver_analysis,
    )


class TestOrbitDeterminationSolverAnalysis:
    def test_constructor(
        self,
        analysis: OrbitDeterminationSolver.Analysis,
    ):
        assert isinstance(analysis, OrbitDeterminationSolver.Analysis)

    def test_properties(
        self,
        analysis: OrbitDeterminationSolver.Analysis,
    ):
        assert isinstance(analysis.estimated_state, State)
        assert isinstance(analysis.solver_analysis, LeastSquaresSolver.Analysis)


class TestOrbitDeterminationSolver:
    def test_constructor(
        self,
        orbit_determination_solver: OrbitDeterminationSolver,
    ):
        assert isinstance(orbit_determination_solver, OrbitDeterminationSolver)

    def test_access_methods(
        self,
        orbit_determination_solver: OrbitDeterminationSolver,
    ):
        assert isinstance(orbit_determination_solver.access_environment(), Environment)
        assert isinstance(orbit_determination_solver.access_propagator(), Propagator)
        assert isinstance(orbit_determination_solver.access_solver(), LeastSquaresSolver)
        assert isinstance(orbit_determination_solver.access_estimation_frame(), Frame)

    def test_estimate(
        self,
        orbit_determination_solver: OrbitDeterminationSolver,
        initial_guess: State,
        observations: list[State],
        coordinate_subsets: list[CoordinateSubset],
        initial_guess_sigmas: dict[CoordinateSubset, list[float]],
        observation_sigmas: dict[CoordinateSubset, list[float]],
    ):
        analysis: OrbitDeterminationSolver.Analysis = orbit_determination_solver.estimate(
            initial_guess=initial_guess,
            observations=observations,
            estimation_coordinate_subsets=coordinate_subsets,
            initial_guess_sigmas=initial_guess_sigmas,
            observation_sigmas=observation_sigmas,
        )

        assert isinstance(analysis, OrbitDeterminationSolver.Analysis)
        assert isinstance(analysis.estimated_state, State)
        assert isinstance(analysis.solver_analysis, LeastSquaresSolver.Analysis)

    def test_estimate_orbit(
        self,
        orbit_determination_solver: OrbitDeterminationSolver,
        initial_guess: State,
        observations: list[State],
        coordinate_subsets: list[CoordinateSubset],
        initial_guess_sigmas: dict[CoordinateSubset, list[float]],
        observation_sigmas: dict[CoordinateSubset, list[float]],
    ):
        orbit: Orbit = orbit_determination_solver.estimate_orbit(
            initial_guess=initial_guess,
            observations=observations,
            estimation_coordinate_subsets=coordinate_subsets,
            initial_guess_sigmas=initial_guess_sigmas,
            observation_sigmas=observation_sigmas,
        )

        assert isinstance(orbit, Orbit)
