# Apache License 2.0

import pytest

import pandas as pd

from ostk.core.type import Real
from ostk.core.type import Integer

from ostk.physics.coordinate import Frame

from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import StateBuilder
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE
from ostk.astrodynamics.estimator import TLESolver
from ostk.astrodynamics.dataframe import generate_states_from_dataframe


@pytest.fixture
def least_squares_solver() -> LeastSquaresSolver:
    return LeastSquaresSolver.default()


@pytest.fixture
def tle_solver(least_squares_solver: LeastSquaresSolver) -> TLESolver:
    return TLESolver(
        solver=least_squares_solver,
        satellite_number=25544,  # ISS NORAD ID
        international_designator="98067A",  # ISS Int'l Designator
        revolution_number=12345,
        fit_with_bstar=True,
    )


@pytest.fixture
def initial_tle() -> TLE:
    return TLE(
        "1 25544U 98067A   22253.00000622  .00000000  00000-0  71655-1 0    02",
        "2 25544  97.5641  21.8296 0012030 155.5301 309.4836 15.14446734123455",
    )


@pytest.fixture
def initial_state(observations: list[State]) -> State:
    return observations[0]


@pytest.fixture
def initial_state_with_bstar(observations: list[State]) -> tuple[State, float]:
    return observations[0], 1e-4


@pytest.fixture
def observations() -> list[State]:
    return generate_states_from_dataframe(
        pd.read_csv("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/gnss_data.csv"),
        reference_frame=Frame.ITRF(),
    )


class TestTLESolver:
    def test_constructor(
        self,
        tle_solver: TLESolver,
    ):
        assert isinstance(tle_solver, TLESolver)
        assert tle_solver.access_satellite_number() == 25544
        assert tle_solver.access_international_designator() == "98067A"
        assert tle_solver.access_revolution_number() == 12345
        assert tle_solver.access_fit_with_bstar() is True

    def test_constructor_defaults(self):
        solver = TLESolver()
        assert solver.access_satellite_number() == 0
        assert solver.access_international_designator() == "00001A"
        assert solver.access_revolution_number() == 0
        assert solver.access_fit_with_bstar() is True

    def test_access_methods(self, tle_solver: TLESolver):
        assert isinstance(tle_solver.access_solver(), LeastSquaresSolver)
        assert isinstance(tle_solver.access_default_bstar(), Real)
        assert isinstance(
            tle_solver.access_first_derivative_mean_motion_divided_by_2(), Real
        )
        assert isinstance(
            tle_solver.access_second_derivative_mean_motion_divided_by_6(), Real
        )
        assert isinstance(tle_solver.access_ephemeris_type(), Integer)
        assert isinstance(tle_solver.access_element_set_number(), Integer)
        assert isinstance(tle_solver.access_tle_state_builder(), StateBuilder)

    def test_estimate_tle_from_tle(
        self,
        tle_solver: TLESolver,
        initial_tle: TLE,
        observations: list[State],
    ):
        analysis: TLESolver.Analysis = tle_solver.estimate_tle(
            initial_guess=initial_tle,
            observations=observations,
        )

        assert isinstance(analysis, TLESolver.Analysis)
        assert isinstance(analysis.determined_tle, TLE)
        assert isinstance(analysis.solver_analysis, LeastSquaresSolver.Analysis)

        assert analysis.solver_analysis.termination_criteria == "RMS Update Threshold"

    def test_estimate_tle_from_state_bstar(
        self,
        tle_solver: TLESolver,
        initial_state: State,
        observations: list[State],
    ):
        analysis: TLESolver.Analysis = tle_solver.estimate_tle(
            initial_guess=(initial_state, 1e-4),
            observations=observations,
        )
        assert isinstance(analysis, TLESolver.Analysis)
        assert isinstance(analysis.determined_tle, TLE)
        assert isinstance(analysis.solver_analysis, LeastSquaresSolver.Analysis)

        assert analysis.solver_analysis.termination_criteria == "RMS Update Threshold"

    def test_estimate_tle_from_state(
        self,
        initial_state: State,
        observations: list[State],
    ):
        tle_solver_no_bstar = TLESolver(
            satellite_number=25544,
            international_designator="98067A",
            revolution_number=12345,
            fit_with_bstar=False,
        )
        analysis: TLESolver.Analysis = tle_solver_no_bstar.estimate_tle(
            initial_guess=initial_state,
            observations=observations,
        )
        assert isinstance(analysis, TLESolver.Analysis)
        assert isinstance(analysis.determined_tle, TLE)
        assert isinstance(analysis.solver_analysis, LeastSquaresSolver.Analysis)

        assert analysis.solver_analysis.termination_criteria == "RMS Update Threshold"

    def test_estimate_tle_invalid_initial_guess(
        self,
        tle_solver: TLESolver,
        observations: list[State],
    ):
        with pytest.raises(RuntimeError) as e:
            tle_solver.estimate_tle(initial_guess="invalid", observations=observations)
        assert "Initial guess must be a TLE, (State, float) tuple, or State." in str(
            e.value
        )

    def test_estimate_tle_invalid_state_only(
        self,
        tle_solver: TLESolver,
        initial_state: State,
        observations: list[State],
    ):
        with pytest.raises(RuntimeError) as e:
            tle_solver.estimate_tle(
                initial_guess=initial_state, observations=observations
            )
        assert "Initial guess must be a TLE or (State, B*) when fitting with B*." in str(
            e.value
        )

    def test_estimate_tle_no_observations(
        self,
        tle_solver: TLESolver,
        initial_state: State,
    ):
        with pytest.raises(Exception):
            tle_solver.estimate_tle(initial_guess=initial_state, observations=[])

    def test_fit_with_different_frames(
        self,
        tle_solver: TLESolver,
        initial_state_with_bstar: tuple[State, float],
        observations: list[State],
    ):
        # Convert observations to TEME frame
        teme_observations = [obs.in_frame(Frame.TEME()) for obs in observations]

        analysis: TLESolver.Analysis = tle_solver.estimate_tle(
            initial_guess=initial_state_with_bstar, observations=teme_observations
        )
        assert isinstance(analysis, TLESolver.Analysis)
        assert isinstance(analysis.determined_tle, TLE)

        assert analysis.solver_analysis.termination_criteria == "RMS Update Threshold"
