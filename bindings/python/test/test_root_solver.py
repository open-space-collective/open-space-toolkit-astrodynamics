# Apache License 2.0

import pytest
import math

from ostk.astrodynamics import RootSolver


def quadratic_function(x):
    return x**2 - 4


@pytest.fixture
def maximum_iteration_count() -> int:
    return 100


@pytest.fixture
def digit_count() -> int:
    return 27


@pytest.fixture
def root_solver(maximum_iteration_count: int, digit_count: int) -> RootSolver:
    return RootSolver(maximum_iteration_count, digit_count)


class TestRootSolver:
    def test_constructor(self, maximum_iteration_count: int, digit_count: int):
        assert RootSolver(maximum_iteration_count, digit_count) is not None

    def test_solve_with_initial_guess(self, root_solver):
        solution = root_solver.solve(quadratic_function, 1.0, True)
        assert pytest.approx(solution.root, abs=1e-15) == 2.0

    def test_default(self):
        assert RootSolver.default() is not None

    def test_getters(
        self,
        root_solver: RootSolver,
        maximum_iteration_count: int,
        digit_count: int,
    ):
        assert root_solver.get_maximum_iteration_count() == maximum_iteration_count
        assert root_solver.get_digit_count() == digit_count
