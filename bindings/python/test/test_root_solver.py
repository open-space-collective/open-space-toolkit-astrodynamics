# Apache License 2.0

import pytest

from ostk.astrodynamics import RootSolver


def quadratic_function(x):
    return x**2 - 4


@pytest.fixture
def maximum_iteration_count() -> int:
    return 100


@pytest.fixture
def tolerance() -> float:
    return 1e-15


@pytest.fixture
def root_solver(maximum_iteration_count: int, tolerance: float) -> RootSolver:
    return RootSolver(maximum_iteration_count, tolerance)


class TestRootSolver:
    def test_constructor(self, maximum_iteration_count: int, tolerance: float):
        assert (
            RootSolver(
                maximum_iteration_count=maximum_iteration_count, tolerance=tolerance
            )
            is not None
        )

    def test_getters(
        self,
        root_solver: RootSolver,
        maximum_iteration_count: int,
        tolerance: float,
    ):
        assert root_solver.get_tolerance() == tolerance
        assert root_solver.get_maximum_iteration_count() == maximum_iteration_count

    def test_bracket_and_solve(self, root_solver):
        solution = root_solver.bracket_and_solve(
            function=quadratic_function,
            initial_guess=1.0,
            is_rising=True,
        )
        assert pytest.approx(solution.root, abs=1e-15) == 2.0

    def test_solve(self, root_solver):
        solution = root_solver.solve(
            function=quadratic_function,
            lower_bound=1.0,
            upper_bound=3.0,
        )
        assert pytest.approx(solution.root, abs=1e-15) == 2.0

    def test_bisection(self, root_solver):
        solution = root_solver.bisection(
            function=quadratic_function,
            lower_bound=1.0,
            upper_bound=3.0,
        )
        assert pytest.approx(solution.root, abs=1e-15) == 2.0

    def test_default(self):
        assert RootSolver.default() is not None
